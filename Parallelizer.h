#ifndef PARALLELIZER_H
#define PARALLELIZER_H

#include <QObject>


class Parallelizer : public QObject
{
    Q_OBJECT
public:
    explicit Parallelizer(QObject *parent = nullptr);
    static QString addOpenMPPragmas(const QString &code);

private:
    // Returns number of nested loops starting at 'startIndex'
    static int countNestedLoops(const QStringList &lines, int startIndex);
    // Returns pragma string based on nested loop count
    static QString buildPragma(int nestedLoopCount);
    static QString buildPragma(int nestedLoopCount, const QSet<QString>& reductions);
    static bool containsUnsafeCalls(const QStringList &lines, int startIndex);
    static QSet<QString> detectUpdatedVariables(const QStringList &lines, int startIndex, const QSet<QString> &loopVars);
    static QSet<QString> extractLoopVariables(const QStringList &lines, int startIndex, int nestedLoopCount);

signals:

};

#endif // PARALLELIZER_H
