// Copyright (C) 2016 Research In Motion
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
#include <qtest.h>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <qqml.h>
#include <QMetaMethod>
#include <setjmp.h>

#include <QtQuickTestUtils/private/qmlutils_p.h>

class ExportedClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int qmlObjectProp READ qmlObjectProp NOTIFY qmlObjectPropChanged)
    Q_PROPERTY(int cppObjectProp READ cppObjectProp NOTIFY cppObjectPropChanged)
    Q_PROPERTY(int unboundProp READ unboundProp NOTIFY unboundPropChanged)
    Q_PROPERTY(int v8BindingProp READ v8BindingProp NOTIFY v8BindingPropChanged)
    Q_PROPERTY(int v4BindingProp READ v4BindingProp NOTIFY v4BindingPropChanged)
    Q_PROPERTY(int v4BindingProp2 READ v4BindingProp2 NOTIFY v4BindingProp2Changed)
    Q_PROPERTY(int scriptBindingProp READ scriptBindingProp NOTIFY scriptBindingPropChanged)
public:
    int qmlObjectPropConnections = 0;
    int cppObjectPropConnections = 0;
    int unboundPropConnections = 0;
    int v8BindingPropConnections = 0;
    int v4BindingPropConnections = 0;
    int v4BindingProp2Connections = 0;
    int scriptBindingPropConnections = 0;
    int boundSignalConnections = 0;
    int unusedSignalConnections = 0;

    ExportedClass() {}

    ~ExportedClass()
    {
        QCOMPARE(qmlObjectPropConnections, 0);
        QCOMPARE(cppObjectPropConnections, 0);
        QCOMPARE(unboundPropConnections, 0);
        QCOMPARE(v8BindingPropConnections, 0);
        QCOMPARE(v4BindingPropConnections, 0);
        QCOMPARE(v4BindingProp2Connections, 0);
        QCOMPARE(scriptBindingPropConnections, 0);
        QCOMPARE(boundSignalConnections, 0);
        QCOMPARE(unusedSignalConnections, 0);
    }

    int qmlObjectProp() const { return 42; }
    int unboundProp() const { return 42; }
    int v8BindingProp() const { return 42; }
    int v4BindingProp() const { return 42; }
    int v4BindingProp2() const { return 42; }
    int cppObjectProp() const { return 42; }
    int scriptBindingProp() const { return 42; }

    void verifyReceiverCount()
    {
        //Note: QTBUG-34829 means we can't call this from within disconnectNotify or it can lock
        QCOMPARE(receivers(SIGNAL(qmlObjectPropChanged())), qmlObjectPropConnections);
        QCOMPARE(receivers(SIGNAL(cppObjectPropChanged())), cppObjectPropConnections);
        QCOMPARE(receivers(SIGNAL(unboundPropChanged())), unboundPropConnections);
        QCOMPARE(receivers(SIGNAL(v8BindingPropChanged())), v8BindingPropConnections);
        QCOMPARE(receivers(SIGNAL(v4BindingPropChanged())), v4BindingPropConnections);
        QCOMPARE(receivers(SIGNAL(v4BindingProp2Changed())), v4BindingProp2Connections);
        QCOMPARE(receivers(SIGNAL(scriptBindingPropChanged())), scriptBindingPropConnections);
        QCOMPARE(receivers(SIGNAL(boundSignal())), boundSignalConnections);
        QCOMPARE(receivers(SIGNAL(unusedSignal())), unusedSignalConnections);
    }

protected:
    void connectNotify(const QMetaMethod &signal) override {
        if (signal.name() == "qmlObjectPropChanged") qmlObjectPropConnections++;
        if (signal.name() == "cppObjectPropChanged") cppObjectPropConnections++;
        if (signal.name() == "unboundPropChanged") unboundPropConnections++;
        if (signal.name() == "v8BindingPropChanged") v8BindingPropConnections++;
        if (signal.name() == "v4BindingPropChanged") v4BindingPropConnections++;
        if (signal.name() == "v4BindingProp2Changed") v4BindingProp2Connections++;
        if (signal.name() == "scriptBindingPropChanged") scriptBindingPropConnections++;
        if (signal.name() == "boundSignal")   boundSignalConnections++;
        if (signal.name() == "unusedSignal") unusedSignalConnections++;
        verifyReceiverCount();
        //qDebug() << Q_FUNC_INFO << this << signal.name();
    }

    void disconnectNotify(const QMetaMethod &signal) override {
        if (signal.name() == "qmlObjectPropChanged") qmlObjectPropConnections--;
        if (signal.name() == "cppObjectPropChanged") cppObjectPropConnections--;
        if (signal.name() == "unboundPropChanged") unboundPropConnections--;
        if (signal.name() == "v8BindingPropChanged") v8BindingPropConnections--;
        if (signal.name() == "v4BindingPropChanged") v4BindingPropConnections--;
        if (signal.name() == "v4BindingProp2Changed") v4BindingProp2Connections--;
        if (signal.name() == "scriptBindingPropChanged") scriptBindingPropConnections--;
        if (signal.name() == "boundSignal")   boundSignalConnections--;
        if (signal.name() == "unusedSignal") unusedSignalConnections--;
        //qDebug() << Q_FUNC_INFO << this << signal.methodSignature();
    }

signals:
    void qmlObjectPropChanged();
    void cppObjectPropChanged();
    void unboundPropChanged();
    void v8BindingPropChanged();
    void v4BindingPropChanged();
    void v4BindingProp2Changed();
    void scriptBindingPropChanged();
    void boundSignal();
    void unusedSignal();
};

class tst_qqmlnotifier : public QQmlDataTest
{
    Q_OBJECT
public:
    tst_qqmlnotifier() : QQmlDataTest(QT_QMLTEST_DATADIR) {}

private slots:
    void initTestCase() override;
    void cleanupTestCase();
    void testConnectNotify();

    void removeV4Binding();
    void removeV4Binding2();
    void removeV8Binding();
    void removeScriptBinding();
    // No need to test value type proxy bindings - the user can't override disconnectNotify() anyway,
    // as the classes are private to the QML engine

    void readProperty();
    void propertyChange();
    void disconnectOnDestroy();
    void lotsOfBindings();

    void deleteFromHandler();

private:
    void createObjects();

    QQmlEngine engine;
    QObject *root = nullptr;
    ExportedClass *exportedClass = nullptr;
    ExportedClass *exportedObject = nullptr;
};

void tst_qqmlnotifier::initTestCase()
{
    QQmlDataTest::initTestCase();
    qmlRegisterType<ExportedClass>("Test", 1, 0, "ExportedClass");
}

void tst_qqmlnotifier::createObjects()
{
    delete root;
    root = nullptr;
    exportedClass = exportedObject = nullptr;

    QQmlComponent component(&engine, testFileUrl("connectnotify.qml"));
    exportedObject = new ExportedClass();
    exportedObject->setObjectName("exportedObject");
    root = component.createWithInitialProperties({{"exportedObject", QVariant::fromValue(exportedObject)}});
    QVERIFY(root != nullptr);

    exportedClass = qobject_cast<ExportedClass *>(
                root->findChild<ExportedClass*>("exportedClass"));
    QVERIFY(exportedClass != nullptr);
    exportedClass->verifyReceiverCount();
}

void tst_qqmlnotifier::cleanupTestCase()
{
    delete root;
    root = nullptr;
    delete exportedObject;
    exportedObject = nullptr;
}

void tst_qqmlnotifier::testConnectNotify()
{
    createObjects();

    QCOMPARE(exportedClass->qmlObjectPropConnections, 1);
    QCOMPARE(exportedClass->cppObjectPropConnections, 0);
    QCOMPARE(exportedClass->unboundPropConnections, 0);
    QCOMPARE(exportedClass->v8BindingPropConnections, 1);
    QCOMPARE(exportedClass->v4BindingPropConnections, 1);
    QCOMPARE(exportedClass->v4BindingProp2Connections, 2);
    QCOMPARE(exportedClass->scriptBindingPropConnections, 1);
    QCOMPARE(exportedClass->boundSignalConnections, 1);
    QCOMPARE(exportedClass->unusedSignalConnections, 0);
    exportedClass->verifyReceiverCount();

    QCOMPARE(exportedObject->qmlObjectPropConnections, 0);
    QCOMPARE(exportedObject->cppObjectPropConnections, 1);
    QCOMPARE(exportedObject->unboundPropConnections, 0);
    QCOMPARE(exportedObject->v8BindingPropConnections, 0);
    QCOMPARE(exportedObject->v4BindingPropConnections, 0);
    QCOMPARE(exportedObject->v4BindingProp2Connections, 0);
    QCOMPARE(exportedObject->scriptBindingPropConnections, 0);
    QCOMPARE(exportedObject->boundSignalConnections, 0);
    QCOMPARE(exportedObject->unusedSignalConnections, 0);
    exportedObject->verifyReceiverCount();
}

void tst_qqmlnotifier::removeV4Binding()
{
    createObjects();

    // Removing a binding should disconnect all of its guarded properties
    QVERIFY(QMetaObject::invokeMethod(root, "removeV4Binding"));
    QCOMPARE(exportedClass->v4BindingPropConnections, 0);
    exportedClass->verifyReceiverCount();
}

void tst_qqmlnotifier::removeV4Binding2()
{
    createObjects();

    // In this case, the v4BindingProp2 property is used by two v4 bindings.
    // Make sure that removing one binding doesn't by accident disconnect all.
    QVERIFY(QMetaObject::invokeMethod(root, "removeV4Binding2"));
    QCOMPARE(exportedClass->v4BindingProp2Connections, 1);
    exportedClass->verifyReceiverCount();
}

void tst_qqmlnotifier::removeV8Binding()
{
    createObjects();

    // Removing a binding should disconnect all of its guarded properties
    QVERIFY(QMetaObject::invokeMethod(root, "removeV8Binding"));
    QCOMPARE(exportedClass->v8BindingPropConnections, 0);
    exportedClass->verifyReceiverCount();
}

void tst_qqmlnotifier::removeScriptBinding()
{
    createObjects();

    // Removing a binding should disconnect all of its guarded properties
    QVERIFY(QMetaObject::invokeMethod(root, "removeScriptBinding"));
    QCOMPARE(exportedClass->scriptBindingPropConnections, 0);
    exportedClass->verifyReceiverCount();
}

void tst_qqmlnotifier::readProperty()
{
    createObjects();

    // Reading a property should not connect to it
    QVERIFY(QMetaObject::invokeMethod(root, "readProperty"));
    QCOMPARE(exportedClass->unboundPropConnections, 0);
    exportedClass->verifyReceiverCount();
}

void tst_qqmlnotifier::propertyChange()
{
    createObjects();

    // Changing the state will trigger the PropertyChange to overwrite a value with a binding.
    // For this, the new binding needs to be connected, and afterwards disconnected.
    QVERIFY(QMetaObject::invokeMethod(root, "changeState"));
    QCOMPARE(exportedClass->unboundPropConnections, 1);
    exportedClass->verifyReceiverCount();
    QVERIFY(QMetaObject::invokeMethod(root, "changeState"));
    QCOMPARE(exportedClass->unboundPropConnections, 0);
    exportedClass->verifyReceiverCount();
}

void tst_qqmlnotifier::disconnectOnDestroy()
{
    createObjects();

    // Deleting a QML object should remove all connections. For exportedClass, this is tested in
    // the destructor, and for exportedObject, it is tested below.
    delete root;
    root = nullptr;
    QCOMPARE(exportedObject->cppObjectPropConnections, 0);
    exportedObject->verifyReceiverCount();
}

class TestObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int a READ a NOTIFY aChanged)

public:
    int a() const { return 0; }

signals:
    void aChanged();
};

void tst_qqmlnotifier::lotsOfBindings()
{
    TestObject o;
    QQmlEngine *e = new QQmlEngine;

    qmlRegisterSingletonInstance("Test", 1, 0, "Test", &o);

    QList<QQmlComponent *> components;
    for (int i = 0; i < 20000; ++i) {
        QQmlComponent *component = new QQmlComponent(e);
        component->setData("import QtQuick 2.0; import Test 1.0; Item {width: Test.a; }", QUrl());
        component->create(e->rootContext());
        components.append(component);
    }

    o.aChanged();

    qDeleteAll(components);
    delete e;
}

void tst_qqmlnotifier::deleteFromHandler()
{
    static jmp_buf jumpBuffer;
    enum {
        LongJmpSetup = 0,
        WrongErrorMessage = 1,
        CorrectErrorMessage = 2
    };
    auto myMessageHandler = [](QtMsgType type, const QMessageLogContext &, const QString &msg) {
        if (type != QtMsgType::QtFatalMsg)
            return;
        if (msg.contains("destroyed while one of its QML signal handlers is in progress"))
            longjmp(jumpBuffer, CorrectErrorMessage);
        else
            longjmp(jumpBuffer, WrongErrorMessage);
    };
    QtMessageHandler defaultHandler = qInstallMessageHandler(myMessageHandler);
    auto cleanup = qScopeGuard([&]() {
        qInstallMessageHandler(defaultHandler);
    });
    QQmlEngine engine;
    QQmlComponent component(&engine, testFileUrl("objectRenamer.qml"));
    QPointer<QObject> mess = component.create();
    QObject::connect(mess.data(), &QObject::objectNameChanged, [&]() { delete mess; });
    switch (setjmp(jumpBuffer)) {
    case CorrectErrorMessage:
        return; // success
    case WrongErrorMessage:
        QFAIL("Did not receive expected fatal warning");
        return;
    case LongJmpSetup:
        break; // longjmp was not called
    default:
        QFAIL("This should never happen");
        return;
    }
    QTRY_VERIFY(mess.isNull()); // BANG!
    QFAIL("Did not receive any fatal warning");
}

QTEST_MAIN(tst_qqmlnotifier)

#include "tst_qqmlnotifier.moc"
