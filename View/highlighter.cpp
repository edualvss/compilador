#include "View/highlighter.h"

HighLighter::HighLighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {

    HighLightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bfloat\\b" << "\\bint\\b" << "\\bbool\\b"
                    << "\\bchar\\b" << "\\breturn\\b" << "\\bvoid\\b"
                    << "\\bbreak\\b" << "\\bif\\b" << "\\belse\\b"
                    << "\\bfor\\b" << "\\bwhile\\b" << "\\bdo\\b"
                    << "\\bswitch\\b";
//                    << "\\bfriend\\b" << "\\binline\\b" << "\\benum\\b" << "\\bexplicit\\b"
//                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
//                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
//                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
//                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
//                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
//                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
//                    << "\\bvoid\\b" << "\\bvolatile\\b"
//                    << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highLightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.format = classFormat;
    highLightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegularExpression("\"([^\n\"]*)\"");
    rule.format = quotationFormat;
    highLightingRules.append(rule);

    numberFormat.setForeground(Qt::magenta);
    rule.pattern = QRegularExpression("\\b([0-9]+|[0-9]+\\.[0-9]+)");
    rule.format = numberFormat;
    highLightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::gray);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highLightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::gray);

    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void HighLighter::highlightBlock(const QString &text) {
    foreach (const HighLightingRule &rule, highLightingRules) {
        QRegularExpression expression(rule.pattern);
/*
        int index = expression.indexIn(text);
        while(index >= 0) {
            int length = expression.matchedLength();
            setFormat(index,length,rule.format);
            index = expression.indexIn(text,index+length);
        }
*/
        QRegularExpressionMatchIterator it = expression.globalMatch(text);
        QRegularExpressionMatch matchIndex = expression.match(text);
        int index = matchIndex.capturedStart();
        while(it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            int lenght = match.capturedLength();
            setFormat(index,lenght,rule.format);
            index += lenght;
        }
    }

    setCurrentBlockState(0);
    int startIndex = 0;
    if(previousBlockState() != 1) {
        QRegularExpressionMatch match = commentStartExpression.match(text);
        startIndex = match.capturedStart(1);
    }
    while(startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text,startIndex);
        int endIndex = match.capturedEnd(1);
        int commentLength;
        if(endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex,commentLength,multiLineCommentFormat);
        startIndex = commentStartExpression.match(text,startIndex+commentLength).capturedStart();
    }
}
