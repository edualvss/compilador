#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class HighLighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    HighLighter(QTextDocument* parent = 0);
protected:
    void highlightBlock(const QString &text);

private:
    struct HighLightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighLightingRule> highLightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat numberFormat;
};

#endif // HIGHLIGHTER_H
