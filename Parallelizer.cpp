#include "Parallelizer.h"
#include <QSet>
#include <QList>
#include <QStringList>
#include <QRegularExpression>

Parallelizer::Parallelizer(QObject *parent) : QObject{parent}
{
}


QSet<QString> Parallelizer::extractLoopVariables(const QStringList &lines, int startIndex, int nestedLoopCount)
{
    QSet<QString> loopVars;

    for (int i = 0; i < nestedLoopCount && (startIndex + i) < lines.size(); ++i) {
        QString line = lines[startIndex + i].trimmed();
        QRegularExpression rx(R"(for\s*\(\s*(?:int|size_t|auto)?\s*(\w+))");
        QRegularExpressionMatch match = rx.match(line);
        if (match.hasMatch()) {
            loopVars.insert(match.captured(1));
        }
    }

    return loopVars;
}


QSet<QString> Parallelizer::detectUpdatedVariables(const QStringList &lines, int startIndex , const QSet<QString> &loopVars)
{
    QSet<QString> updatedVars;
    int braceCount = 0;
    bool loopStarted = false;

    for (int i = startIndex; i < lines.size(); ++i) {
        QString line = lines[i];

        // Count braces to track loop body
        for (QChar ch : line) {
            if (ch == '{') braceCount++;
            else if (ch == '}') braceCount--;
        }

        QString trimmed = line.trimmed();

        if (!loopStarted && line.contains("{")) {
            loopStarted = true;
        }

        // Stop scanning when loop body ends
        if (loopStarted && braceCount <= 0) {
            break;
        }

        // Detect increments/additions: var++ or var +=
        QRegularExpression rxInc(R"((\w+)\s*(\+\+|(\+=)))");

        QRegularExpressionMatch matchInc = rxInc.match(trimmed);

        if (matchInc.hasMatch()) {
            QString varName = matchInc.captured(1);
            if (!loopVars.contains(varName))  // <- Exclude loop variables
                updatedVars.insert(varName);
        }

        // Detect assignments like var = var + ...
        QRegularExpression rxAssign(R"((\w+)\s*=\s*\1\s*\+)");
        QRegularExpressionMatch matchAssign = rxAssign.match(trimmed);
        if (matchAssign.hasMatch()) {
            QString varName = matchAssign.captured(1);
            if (!loopVars.contains(varName))
                updatedVars.insert(varName);
        }

    }

    return updatedVars;
}




// Scan the entire loop body starting at startIndex by matching braces.
// Return true if any line inside the loop body contains "cout" or "std::cout".
bool Parallelizer::containsUnsafeCalls(const QStringList &lines, int startIndex)
{
    int totalLines = lines.size();
    int braceCount = 0;
    bool loopStarted = false;

    for (int i = startIndex; i < totalLines; ++i) {
        QString line = lines[i];

        // Count braces to detect loop body scope
        for (QChar ch : line) {
            if (ch == '{') braceCount++;
            else if (ch == '}') braceCount--;
        }

        QString trimmed = line.trimmed();
        // Detect unsafe calls inside loop body
        if (trimmed.contains("cout") || trimmed.contains("std::cout")) {
            return true;
        }


        // Mark loop body start when first '{' is encountered
        if (!loopStarted && line.contains("{")) {
            loopStarted = true;
        }

        // If loop body started and all braces closed, end scanning
        if (loopStarted && braceCount <= 0) {
            break;
        }
    }
    return false;
}

int Parallelizer::countNestedLoops(const QStringList &lines, int startIndex)
{
    int collapseCount = 0;
    int baseIndent = -1;
    int totalLines = lines.size();

    for (int i = startIndex; i < totalLines; ++i) {
        QString line = lines[i];
        QString trimmed = line.trimmed();

        if (trimmed.startsWith("for(") || trimmed.startsWith("for (")) {
            int indent = line.indexOf(trimmed);
            if (baseIndent == -1 || indent > baseIndent) {
                baseIndent = indent;
                collapseCount++;
            } else {
                break; // Not nested deeper
            }
        } else if (!trimmed.isEmpty()) {
            break; // Stop if line is not a for loop or empty
        }
    }
    return collapseCount;
}



QString Parallelizer::buildPragma(int nestedLoopCount, const QSet<QString>& reductions)
{
    QString pragma;

    if (nestedLoopCount <= 1)
        pragma = "#pragma omp parallel for";
    else
        pragma = QString("#pragma omp parallel for collapse(%1)").arg(nestedLoopCount);

    if (!reductions.isEmpty()) {
        QStringList vars = reductions.values();  // Correct conversion here
        pragma += " reduction(+: " + vars.join(", ") + ")";
    }

    pragma += "\n";
    return pragma;
}


QString Parallelizer::addOpenMPPragmas(const QString &code)
{
    QStringList lines = code.split('\n');
    QString result;

    int i = 0;
    while (i < lines.size()) {
        QString line = lines[i];
        QString trimmed = line.trimmed();



        if (trimmed.startsWith("for(") || trimmed.startsWith("for (")) {
            int nestedLoops = countNestedLoops(lines, i);
            QSet<QString> loopVars = extractLoopVariables(lines, i, nestedLoops);

            if (containsUnsafeCalls(lines, i)) {
                for (int j = 0; j < nestedLoops && (i + j) < lines.size(); ++j) {
                    result += lines[i + j] + '\n';
                }
                i += nestedLoops;
                continue;
            }

            // Detect updated vars for reduction
            QSet<QString> reductions = detectUpdatedVariables(lines, i ,loopVars);

            // Build pragma with reductions
            result += buildPragma(nestedLoops, reductions);

            for (int j = 0; j < nestedLoops && (i + j) < lines.size(); ++j) {
                result += lines[i + j] + '\n';
            }
            i += nestedLoops;
        } else {
            result += line + '\n';
            i++;
        }
    }
    return result;
}
