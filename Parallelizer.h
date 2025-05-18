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
    static bool containsUnsafeCalls(const QStringList &lines, int startIndex, int nestedLoopCount);

signals:

};

#endif // PARALLELIZER_H
