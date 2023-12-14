// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QFile>
#include <private/qquickrectangle_p.h>
#include <private/qsvgtinydocument_p.h>
#include <QQuickWindow>

#include "qsvgloader_p.h"

#define ENABLE_GUI

int main(int argc, char *argv[])
{
#ifdef ENABLE_GUI
    qputenv("QT_QUICKSHAPES_BACKEND", "curverenderer");
    QGuiApplication app(argc, argv);
#else
    QCoreApplication app(argc, argv);
#endif

    QCommandLineParser parser;
    parser.setApplicationDescription("SVG to QML converter [tech preview]");
    parser.addHelpOption();
    parser.addPositionalArgument("input", QCoreApplication::translate("main", "SVG file to read."));
    parser.addPositionalArgument("output", QCoreApplication::translate("main", "QML file to write."));

#if 0
    QCommandLineOption separateOption(QStringList() << "s" << "separate-items",
                                      QCoreApplication::translate("main", "Generate separate items for all sub-shapes."));
    parser.addOption(separateOption);

    QCommandLineOption combineOption(QStringList() << "c" << "combine-shapes",
                                     QCoreApplication::translate("main", "Combine all sub-shapes into one item."));
    parser.addOption(combineOption);
#endif
    QCommandLineOption typeNameOption(QStringList() << "t" << "type-name",
                                      QCoreApplication::translate("main", "Use <typename> for Shape."),
                                      QCoreApplication::translate("main", "typename"));
    parser.addOption(typeNameOption);


#ifdef ENABLE_GUI
    QCommandLineOption guiOption(QStringList() << "v" << "view",
                                      QCoreApplication::translate("main", "Display the SVG in a window."));
    parser.addOption(guiOption);
#endif
    parser.process(app);
    const QStringList args = parser.positionalArguments();
    if (args.size() < 1) {
        parser.showHelp(1);
    }

    auto *doc = QSvgTinyDocument::load(args.at(0));
    if (!doc) {
        fprintf(stderr, "%s is not a valid SVG\n", qPrintable(args.at(0)));
        return 2;
    }

    auto outFileName = args.size() > 1 ? args.at(1) : QString{};
    auto typeName = parser.value(typeNameOption);

#ifdef ENABLE_GUI
    if (parser.isSet(guiOption)) {
        app.setOrganizationName("QtProject");
        const QUrl url(QStringLiteral("qrc:/main.qml"));
        QQmlApplicationEngine engine;
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                         &app, [url, outFileName, doc, typeName](QObject *obj, const QUrl &objUrl){
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
            if (obj) {
                auto *containerItem = obj->findChild<QQuickItem*>(QStringLiteral("svg_item"));
                auto *contents = QSvgQmlWriter::loadSVG(doc, outFileName, typeName, containerItem);
                contents->setWidth(containerItem->implicitWidth()); // Workaround for runtime loader viewbox size logic. TODO: fix
                contents->setHeight(containerItem->implicitHeight());
            }
        });
        engine.load(url);
        return app.exec();
    }
#endif

    QSvgQmlWriter::loadSVG(doc, outFileName, typeName, nullptr);
    return 0;
}