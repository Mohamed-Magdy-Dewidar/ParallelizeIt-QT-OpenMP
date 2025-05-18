#include "Parallelizer.h"

Parallelizer::Parallelizer(QObject *parent) : QObject{parent}
{
}









// Scan the entire loop body starting at startIndex by matching braces.
// Return true if any line inside the loop body contains "cout" or "std::cout".
bool Parallelizer::containsUnsafeCalls(const QStringList &lines, int startIndex, int /*nestedLoopCount*/)
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

QString Parallelizer::buildPragma(int nestedLoopCount)
{
    if (nestedLoopCount <= 1)
        return "#pragma omp parallel for\n";
    else
        return QString("#pragma omp parallel for collapse(%1)\n").arg(nestedLoopCount);
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

            // Use improved detection to skip loops with unsafe calls
            if (containsUnsafeCalls(lines, i, nestedLoops)) {
                // Output loops as-is without pragma
                for (int j = 0; j < nestedLoops && (i + j) < lines.size(); ++j) {
                    result += lines[i + j] + '\n';
                }
                i += nestedLoops;
                continue;
            }

            // Otherwise, insert the appropriate pragma
            result += buildPragma(nestedLoops);
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
