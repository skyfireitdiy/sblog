let markdown_converter = function () {
    converter = new showdown.Converter({
        extensions: function () {
            function htmlunencode(text) {
                return (
                    text
                        .replace(/&amp;/g, '&')
                        .replace(/&lt;/g, '<')
                        .replace(/&gt;/g, '>')
                );
            }

            return [
                {
                    type: 'output',
                    filter: function (text, converter, options) {
                        // use new shodown's regexp engine to conditionally parse codeblocks
                        var left = '<pre><code\\b[^>]*>',
                            right = '</code></pre>',
                            flags = 'g',
                            replacement = function (wholeMatch, match, left, right) {
                                // unescape match to prevent double escaping
                                match = htmlunencode(match);
                                return left + hljs.highlightAuto(match).value + right;
                            };
                        return showdown.helper.replaceRecursiveRegExp(text, replacement, left, right, flags);
                    }
                }
            ];
        }()
    });

    converter.setOption("omitExtraWLInCodeBlocks", true);
    converter.setOption("ghCompatibleHeaderId", true);
    converter.setOption("prefixHeaderId", true);
    converter.setOption("headerLevelStart", 1);
    converter.setOption("parseImgDimensions", true);
    converter.setOption("literalMidWordUnderscores", true);
    converter.setOption("tables", true);
    converter.setOption("tablesHeaderId", true);
    converter.setOption("ghCodeBlocks", true);
    converter.setOption("tasklists", true);
    converter.setOption("smartIndentationFix", true);
    converter.setOption("simpleLineBreaks", true);
    converter.setOption("requireSpaceBeforeHeadingText", true);
    converter.setOption("openLinksInNewWindow", true);
    converter.setOption("backslashEscapesHTMLTags", true);
    converter.setOption("emoji", true);
    return converter;
};

let markdown_editor = function (elem_name) {
    ace.require("ace/ext/language_tools");
    editor = ace.edit(elem_name);

    editor.setOption("wrap", "free");
    editor.setOptions({
        enableBasicAutocompletion: true,
        enableSnippets: true,
        enableLiveAutocompletion: true,
        wrap: "free",
        highlightActiveLine: true,
        highlightSelectedWord: true,
        selectionStyle: "text",
        readOnly: false,
        cursorStyle: "ace",
        mergeUndoDeltas: "always",
        behavioursEnabled: true,
        wrapBehavioursEnabled: true,
        autoScrollEditorIntoView: true,
        copyWithEmptySelection: false,
        useSoftTabs: true,
        navigateWithinSoftTabs: true,
        enableMultiselect: true,

        hScrollBarAlwaysVisible: false,
        vScrollBarAlwaysVisible: false,
        highlightGutterLine: true,
        animatedScroll: true,
        showInvisibles: true,
        showPrintMargin: true,
        printMarginColumn: 80,
        printMargin: 80,
        fadeFoldWidgets: true,
        showFoldWidgets: true,
        showLineNumbers: true,
        showGutter: true,
        displayIndentGuides: true,
        fontFamily: "Consolas, Monospace, '微软雅黑', 'Heiti SC', '文泉驿微米黑'",
        fontSize: 13,
        scrollPastEnd: true,
        fixedWidthGutter: true,
        theme: "ace/theme/clouds",

        mode: "ace/mode/markdown",
        dragEnabled: true,
        newLineMode: "auto",
        foldStyle: "markbeginend",
        enableSnippets: true,
        enableEmmet: true,
        useElasticTabstops: true
    });
    return editor;
};