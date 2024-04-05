// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtGui/qpa/qplatformintegration.h>
#include <QtGui/qpa/qplatformtheme.h>
#include <QtGui/private/qguiapplication_p.h>
#include <QtTest>
#include <QtQml>
#include <QtQuickTestUtils/private/qmlutils_p.h>
#include <QtQuickTestUtils/private/visualtestutils_p.h>
#include <QtQuickTemplates2/private/qquickapplicationwindow_p.h>
#include <QtQuickTemplates2/private/qquickbutton_p.h>
#include <QtQuickTemplates2/private/qquickmenu_p.h>
#include <QtQuickTemplates2/private/qquickmenu_p_p.h>
#include <QtQuickTemplates2/private/qquickmenubar_p.h>
#include <QtQuickTemplates2/private/qquickmenubar_p_p.h>
#include <QtQuickTemplates2/private/qquickmenubaritem_p.h>
#include <QtQuickTemplates2/private/qquickmenuitem_p.h>
#include <QtQuickControlsTestUtils/private/controlstestutils_p.h>
#include <QtQuickControlsTestUtils/private/qtest_quickcontrols_p.h>

using namespace QQuickVisualTestUtils;
using namespace QQuickControlsTestUtils;

class tst_qquickmenubar : public QQmlDataTest
{
    Q_OBJECT

public:
    tst_qquickmenubar();

private slots:
    void init();
    void delegate();
    void mouse();
    void touch();
    void keys();
    void mnemonics();
    void altNavigation();
    void addRemove_data();
    void addRemove();
    void addRemoveInlineMenus_data();
    void addRemoveInlineMenus();
    void addRemoveMenuFromQml_data();
    void addRemoveMenuFromQml();
    void insert_data();
    void insert();
    void removeMenuThatIsOpen();
    void addRemoveExistingMenus_data();
    void addRemoveExistingMenus();
    void checkHighlightWhenMenuDismissed();
    void hoverAfterClosingWithEscape();
    void AA_DontUseNativeMenuBar();
    void containerItems_data();
    void containerItems();
    void mixedContainerItems_data();
    void mixedContainerItems();
    void applicationWindow_data();
    void applicationWindow();
    void menubarAsHeader_data();
    void menubarAsHeader();

private:
    static bool hasWindowActivation();
    bool nativeMenuBarSupported = false;
    QScopedPointer<QPointingDevice> touchScreen = QScopedPointer<QPointingDevice>(QTest::createTouchDevice());
};

QPoint itemSceneCenter(const QQuickItem *item)
{
    return item->mapToScene(QPointF(item->width() / 2, item->height() / 2)).toPoint();
}

tst_qquickmenubar::tst_qquickmenubar()
    : QQmlDataTest(QT_QMLTEST_DATADIR)
{
    qputenv("QML_NO_TOUCH_COMPRESSION", "1");
    QQuickMenuBar mb;
    nativeMenuBarSupported = QQuickMenuBarPrivate::get(&mb)->useNativeMenuBar();
}

bool tst_qquickmenubar::hasWindowActivation()
{
    return (QGuiApplicationPrivate::platformIntegration()->hasCapability(QPlatformIntegration::WindowActivation));
}

void tst_qquickmenubar::init()
{
    // Enable non-native menubars by default.
    // Note that some tests will set this property to 'true', which
    // is why we need to set it back to 'false' here.
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, false);
}

void tst_qquickmenubar::delegate()
{
    QQmlApplicationEngine engine(testFileUrl("empty.qml"));
    QScopedPointer<QQuickMenuBar> menuBar(qobject_cast<QQuickMenuBar *>(engine.rootObjects().value(0)));
    QVERIFY(menuBar);

    QQmlComponent *delegate = menuBar->delegate();
    QVERIFY(delegate);

    QScopedPointer<QQuickMenuBarItem> item(qobject_cast<QQuickMenuBarItem *>(delegate->create()));
    QVERIFY(item);
}

void tst_qquickmenubar::mouse()
{
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);

    if (!hasWindowActivation())
        QSKIP("Window activation is not supported");

    if ((QGuiApplication::platformName() == QLatin1String("offscreen"))
        || (QGuiApplication::platformName() == QLatin1String("minimal")))
        QSKIP("Mouse highlight not functional on offscreen/minimal platforms");

    QQmlApplicationEngine engine(testFileUrl("menubaritems.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);

    centerOnScreen(window.data());
    moveMouseAway(window.data());
    QVERIFY(QTest::qWaitForWindowActive(window.data()));

    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    QQuickMenu *fileMenuBarMenu = menuBar->menuAt(0);
    QQuickMenu *editMenuBarMenu = menuBar->menuAt(1);
    QQuickMenu *viewMenuBarMenu = menuBar->menuAt(2);
    QQuickMenu *helpMenuBarMenu = menuBar->menuAt(3);
    QVERIFY(fileMenuBarMenu && editMenuBarMenu && viewMenuBarMenu && helpMenuBarMenu);

    QQuickMenuBarItem *fileMenuBarItem = qobject_cast<QQuickMenuBarItem *>(fileMenuBarMenu->parentItem());
    QQuickMenuBarItem *editMenuBarItem = qobject_cast<QQuickMenuBarItem *>(editMenuBarMenu->parentItem());
    QQuickMenuBarItem *viewMenuBarItem = qobject_cast<QQuickMenuBarItem *>(viewMenuBarMenu->parentItem());
    QQuickMenuBarItem *helpMenuBarItem = qobject_cast<QQuickMenuBarItem *>(helpMenuBarMenu->parentItem());
    QVERIFY(fileMenuBarItem && editMenuBarItem && viewMenuBarItem && helpMenuBarItem);

    // highlight a menubar item
    QTest::mouseMove(window.data(), itemSceneCenter(fileMenuBarItem));
#ifndef Q_OS_ANDROID
    // Android theme does not use hover effects, so moving the mouse would not
    // highlight an item
    QVERIFY(fileMenuBarItem->isHighlighted());
#endif
    QVERIFY(!fileMenuBarMenu->isVisible());

    // highlight another menubar item
    QTest::mouseMove(window.data(), itemSceneCenter(editMenuBarItem));
#ifndef Q_OS_ANDROID
    // Android theme does not use hover effects, so moving the mouse would not
    // highlight an item
    QVERIFY(!fileMenuBarItem->isHighlighted());
    QVERIFY(editMenuBarItem->isHighlighted());
#endif
    QVERIFY(!fileMenuBarMenu->isVisible());
    QVERIFY(!editMenuBarMenu->isVisible());

    // trigger a menubar item to open a menu - it should open on press
    QTest::mousePress(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(editMenuBarItem));
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(editMenuBarMenu->isVisible());
    QTRY_VERIFY(editMenuBarMenu->isOpened());
    QTest::mouseRelease(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(editMenuBarItem));

    // re-trigger a menubar item to hide the menu - it should close on press
    QTest::mousePress(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(editMenuBarItem));
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(editMenuBarItem->hasActiveFocus());
    QTRY_VERIFY(!editMenuBarMenu->isVisible());
    QTest::mouseRelease(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(editMenuBarItem));

    // re-trigger a menubar item to show the menu again
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(editMenuBarItem));
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(editMenuBarMenu->isVisible());
    QTRY_VERIFY(editMenuBarMenu->isOpened());

    // highlight another menubar item to open another menu
    QTest::mouseMove(window.data(), itemSceneCenter(helpMenuBarItem));
#ifdef Q_OS_ANDROID
    // Android theme does not use hover effects, so moving the mouse would not
    // highlight an item. Add a mouse click to change menubar item selection.
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier,
                      helpMenuBarItem->mapToScene(QPointF(helpMenuBarItem->width() / 2,
                                                  helpMenuBarItem->height() / 2)).toPoint());
#endif
    QVERIFY(!fileMenuBarItem->isHighlighted());
    QVERIFY(!editMenuBarItem->isHighlighted());
    QVERIFY(!viewMenuBarItem->isHighlighted());
    QVERIFY(helpMenuBarItem->isHighlighted());
    QVERIFY(!fileMenuBarMenu->isVisible());
    QVERIFY(!viewMenuBarMenu->isVisible());
    QVERIFY(helpMenuBarMenu->isVisible());
    QTRY_VERIFY(!editMenuBarMenu->isVisible());
    QTRY_VERIFY(helpMenuBarMenu->isOpened());

    // trigger a menu item to close the menu
    QQuickMenuItem *aboutMenuItem = qobject_cast<QQuickMenuItem *>(helpMenuBarMenu->itemAt(0));
    QVERIFY(aboutMenuItem);
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, aboutMenuItem->mapToScene(QPointF(aboutMenuItem->width() / 2, aboutMenuItem->height() / 2)).toPoint());
    QVERIFY(!helpMenuBarItem->isHighlighted());
    QTRY_VERIFY(!helpMenuBarMenu->isVisible());

    // highlight a menubar item
    QTest::mouseMove(window.data(), itemSceneCenter(editMenuBarItem));
#ifndef Q_OS_ANDROID
    // Android theme does not use hover effects, so moving the mouse would not
    // highlight an item
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(!helpMenuBarItem->isHighlighted());
#endif
    QVERIFY(!editMenuBarMenu->isVisible());
    QVERIFY(!helpMenuBarMenu->isVisible());

    // trigger a menubar item to open a menu
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(viewMenuBarItem));
    QVERIFY(!editMenuBarItem->isHighlighted());
    QVERIFY(viewMenuBarItem->isHighlighted());
    QVERIFY(viewMenuBarMenu->isVisible());
    QTRY_VERIFY(viewMenuBarMenu->isOpened());

    // trigger a menu item to open a sub-menu
    QQuickMenuItem *alignmentSubMenuItem = qobject_cast<QQuickMenuItem *>(viewMenuBarMenu->itemAt(0));
    QVERIFY(alignmentSubMenuItem);
    QQuickMenu *alignmentSubMenu = alignmentSubMenuItem->subMenu();
    QVERIFY(alignmentSubMenu);
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(alignmentSubMenuItem));
#if !defined(Q_OS_ANDROID) and !defined(Q_OS_WEBOS)
    // The screen on Android is too small to fit the whole hierarchy, so the
    // Alignment sub-menu is shown on top of View menu.
    // WebOS also shows alignment sub-menu on top of View menu.
    QVERIFY(viewMenuBarMenu->isVisible());
#endif
    QVERIFY(alignmentSubMenu->isVisible());
    QTRY_VERIFY(alignmentSubMenu->isOpened());

    // trigger a menu item to open a sub-sub-menu
    QQuickMenuItem *verticalSubMenuItem = qobject_cast<QQuickMenuItem *>(alignmentSubMenu->itemAt(1));
    QVERIFY(verticalSubMenuItem);
    QQuickMenu *verticalSubMenu = verticalSubMenuItem->subMenu();
    QVERIFY(verticalSubMenu);
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(verticalSubMenuItem));
#if !defined(Q_OS_ANDROID) and !defined(Q_OS_WEBOS)
    // The screen on Android is too small to fit the whole hierarchy, so the
    // Vertical sub-menu is shown on top of View menu and Alignment sub-menu.
    // WebOS also shows vertical sub-menu on top of View menu and Alignment sub-menu.
    QVERIFY(viewMenuBarMenu->isVisible());
    QVERIFY(alignmentSubMenu->isVisible());
#endif
    QVERIFY(verticalSubMenu->isVisible());
    QTRY_VERIFY(verticalSubMenu->isOpened());

    // trigger a menu item to close the whole chain of menus
    QQuickMenuItem *centerMenuItem = qobject_cast<QQuickMenuItem *>(verticalSubMenu->itemAt(1));
    QVERIFY(centerMenuItem);
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(centerMenuItem));
    QVERIFY(!viewMenuBarItem->isHighlighted());
    QTRY_VERIFY(!viewMenuBarMenu->isVisible());
    QTRY_VERIFY(!alignmentSubMenu->isVisible());
    QTRY_VERIFY(!verticalSubMenu->isVisible());

    // re-highlight the same menubar item
#ifndef Q_OS_ANDROID
    // Android theme does not use hover effects, so moving the mouse would not
    // highlight an item
    QTest::mouseMove(window.data(), viewMenuBarItem->mapToScene(QPointF(viewMenuBarItem->width() / 2, viewMenuBarItem->height() / 2)).toPoint());
    QVERIFY(viewMenuBarItem->isHighlighted());
#endif

    // re-open the chain of menus
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(viewMenuBarItem));
    QTRY_VERIFY(viewMenuBarMenu->isOpened());
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(alignmentSubMenuItem));
    QTRY_VERIFY(alignmentSubMenu->isOpened());
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, itemSceneCenter(verticalSubMenuItem));
    QTRY_VERIFY(verticalSubMenu->isOpened());

    // click outside to close the whole chain of menus
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier, QPoint(window->width() - 1, window->height() - 1));
    QVERIFY(!viewMenuBarItem->isHighlighted());
    QTRY_VERIFY(!viewMenuBarMenu->isVisible());
    QTRY_VERIFY(!alignmentSubMenu->isVisible());
    QTRY_VERIFY(!verticalSubMenu->isVisible());
}

// Not sure how relevant MenuBar is for touch, but this test is here to make
// sure that only release events cause the menu to open, as:
// - That is how it has always behaved, so maintain that behavior.
// - It's what happens with e.g. overflow menus on Android.
void tst_qquickmenubar::touch()
{
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);
    QQuickControlsApplicationHelper helper(this, QLatin1String("touch.qml"));
    QVERIFY2(helper.ready, helper.failureMessage());
    centerOnScreen(helper.window);
    helper.window->show();
    QVERIFY(QTest::qWaitForWindowExposed(helper.window));

    QQuickMenuBar *menuBar = helper.window->property("header").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    QQuickMenu *fileMenuBarMenu = menuBar->menuAt(0);
    QVERIFY(fileMenuBarMenu);

    QQuickMenuBarItem *fileMenuBarItem = qobject_cast<QQuickMenuBarItem *>(fileMenuBarMenu->parentItem());
    QVERIFY(fileMenuBarItem);

    // Trigger a menubar item to open a menu - it should only open on release.
    QTest::touchEvent(helper.window, touchScreen.data()).press(0, itemSceneCenter(fileMenuBarItem));
    QVERIFY(!fileMenuBarItem->isHighlighted());
    QVERIFY(!fileMenuBarMenu->isVisible());
    QTest::touchEvent(helper.window, touchScreen.data()).release(0, itemSceneCenter(fileMenuBarItem));
    QVERIFY(fileMenuBarItem->isHighlighted());
    QVERIFY(fileMenuBarMenu->isVisible());
    QTRY_VERIFY(fileMenuBarMenu->isOpened());
}

void tst_qquickmenubar::keys()
{
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);
    if (!hasWindowActivation())
        QSKIP("Window activation is not supported");

    QQmlApplicationEngine engine(testFileUrl("menubaritems.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);

    centerOnScreen(window.data());
    moveMouseAway(window.data());
    QVERIFY(QTest::qWaitForWindowActive(window.data()));

    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    QQuickMenu *fileMenuBarMenu = menuBar->menuAt(0);
    QQuickMenu *editMenuBarMenu = menuBar->menuAt(1);
    QQuickMenu *viewMenuBarMenu = menuBar->menuAt(2);
    QQuickMenu *helpMenuBarMenu = menuBar->menuAt(3);
    QVERIFY(fileMenuBarMenu && editMenuBarMenu && viewMenuBarMenu && helpMenuBarMenu);

    QQuickMenuBarItem *fileMenuBarItem = qobject_cast<QQuickMenuBarItem *>(fileMenuBarMenu->parentItem());
    QQuickMenuBarItem *editMenuBarItem = qobject_cast<QQuickMenuBarItem *>(editMenuBarMenu->parentItem());
    QQuickMenuBarItem *viewMenuBarItem = qobject_cast<QQuickMenuBarItem *>(viewMenuBarMenu->parentItem());
    QQuickMenuBarItem *helpMenuBarItem = qobject_cast<QQuickMenuBarItem *>(helpMenuBarMenu->parentItem());
    QVERIFY(fileMenuBarItem && editMenuBarItem && viewMenuBarItem && helpMenuBarItem);

    // trigger a menubar item to open a menu
    editMenuBarItem->forceActiveFocus();
    QTest::keyClick(window.data(), Qt::Key_Space);
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(editMenuBarMenu->isVisible());
    QTRY_VERIFY(editMenuBarMenu->isOpened());
    QVERIFY(editMenuBarMenu->hasActiveFocus());

    // navigate down to the menu
    QQuickMenuItem *cutMenuItem = qobject_cast<QQuickMenuItem *>(editMenuBarMenu->itemAt(0));
    QVERIFY(cutMenuItem);
    QVERIFY(!cutMenuItem->isHighlighted());
    QVERIFY(!cutMenuItem->hasActiveFocus());
    QTest::keyClick(window.data(), Qt::Key_Down);
    QVERIFY(cutMenuItem->isHighlighted());
    QVERIFY(cutMenuItem->hasActiveFocus());

    // navigate up, back to the menubar
    QTest::keyClick(window.data(), Qt::Key_Up);
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(editMenuBarItem->hasActiveFocus());
    QTRY_VERIFY(!editMenuBarMenu->isVisible());

// There seem to be problems in focus handling in webOS QPA, see https://bugreports.qt.io/browse/WEBOSCI-45
#ifdef Q_OS_WEBOS
    QEXPECT_FAIL("", "WEBOSCI-45", Abort);
#endif
    QVERIFY(!cutMenuItem->isHighlighted());
    QVERIFY(!cutMenuItem->hasActiveFocus());

    // navigate down to re-open the menu
    QTest::keyClick(window.data(), Qt::Key_Down);
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(!editMenuBarItem->hasActiveFocus());
    QVERIFY(editMenuBarMenu->isVisible());
    QTRY_VERIFY(editMenuBarMenu->isOpened());
    QVERIFY(editMenuBarMenu->hasActiveFocus());
    QVERIFY(cutMenuItem->isHighlighted());
    QVERIFY(cutMenuItem->hasActiveFocus());

    // navigate left in popup mode (menu open)
    QTest::keyClick(window.data(), Qt::Key_Left);
    QVERIFY(fileMenuBarItem->isHighlighted());
    QVERIFY(!editMenuBarItem->isHighlighted());
    QVERIFY(fileMenuBarMenu->isVisible());
    QTRY_VERIFY(fileMenuBarMenu->isOpened());
    QTRY_VERIFY(!editMenuBarMenu->isVisible());

    // navigate left in popup mode (wrap)
    QTest::keyClick(window.data(), Qt::Key_Left);
    QVERIFY(helpMenuBarItem->isHighlighted());
    QVERIFY(!fileMenuBarItem->isHighlighted());
    QVERIFY(helpMenuBarMenu->isVisible());
    QTRY_VERIFY(helpMenuBarMenu->isOpened());
    QTRY_VERIFY(!fileMenuBarMenu->isVisible());

    // navigate up to close the menu
    QTest::keyClick(window.data(), Qt::Key_Up);
    QVERIFY(helpMenuBarItem->isHighlighted());
    QTRY_VERIFY(!helpMenuBarMenu->isVisible());

    // navigate right in non-popup mode (wrap)
    QTest::keyClick(window.data(), Qt::Key_Right);
    QVERIFY(fileMenuBarItem->isHighlighted());
    QVERIFY(!helpMenuBarItem->isHighlighted());
    QVERIFY(!fileMenuBarMenu->isVisible());
    QVERIFY(!helpMenuBarMenu->isVisible());

    // navigate right in non-popup mode (menu closed)
    QTest::keyClick(window.data(), Qt::Key_Right);
    QVERIFY(!fileMenuBarItem->isHighlighted());
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(!fileMenuBarMenu->isVisible());
    QVERIFY(!editMenuBarMenu->isVisible());

    // open a menu
    viewMenuBarItem->forceActiveFocus();
    QTest::keyClick(window.data(), Qt::Key_Space);
    QVERIFY(viewMenuBarItem->isHighlighted());
    QVERIFY(viewMenuBarMenu->isVisible());
    QTRY_VERIFY(viewMenuBarMenu->isOpened());
    QVERIFY(!viewMenuBarItem->hasActiveFocus());
    QVERIFY(viewMenuBarMenu->hasActiveFocus());

    // open a sub-menu
    QQuickMenuItem *alignmentSubMenuItem = qobject_cast<QQuickMenuItem *>(viewMenuBarMenu->itemAt(0));
    QVERIFY(alignmentSubMenuItem);
    QQuickMenu *alignmentSubMenu = alignmentSubMenuItem->subMenu();
    QVERIFY(alignmentSubMenu);
    QTest::keyClick(window.data(), Qt::Key_Down);
    QVERIFY(alignmentSubMenuItem->isHighlighted());
    QVERIFY(!alignmentSubMenu->isVisible());
    QTest::keyClick(window.data(), Qt::Key_Right);
    QVERIFY(alignmentSubMenu->isVisible());
    QTRY_VERIFY(alignmentSubMenu->isOpened());

    // open a sub-sub-menu
    QQuickMenuItem *horizontalSubMenuItem = qobject_cast<QQuickMenuItem *>(alignmentSubMenu->itemAt(0));
    QVERIFY(horizontalSubMenuItem);
    QVERIFY(horizontalSubMenuItem->isHighlighted());
    QQuickMenu *horizontalSubMenu = horizontalSubMenuItem->subMenu();
    QVERIFY(horizontalSubMenu);
    QTest::keyClick(window.data(), Qt::Key_Right);
    QVERIFY(viewMenuBarMenu->isVisible());
    QVERIFY(alignmentSubMenu->isVisible());
    QVERIFY(horizontalSubMenu->isVisible());
    QTRY_VERIFY(horizontalSubMenu->isOpened());

    // navigate left to close a sub-menu
    QTest::keyClick(window.data(), Qt::Key_Left);
    QTRY_VERIFY(!horizontalSubMenu->isVisible());
    QVERIFY(viewMenuBarMenu->isVisible());
    QVERIFY(alignmentSubMenu->isVisible());

    // navigate right to re-open the sub-menu
    QTest::keyClick(window.data(), Qt::Key_Right);
    QVERIFY(horizontalSubMenuItem->isHighlighted());
    QVERIFY(horizontalSubMenu->isVisible());
    QTRY_VERIFY(horizontalSubMenu->isOpened());

    // navigate right to the next menubar menu
    QTest::keyClick(window.data(), Qt::Key_Right);
    QVERIFY(!viewMenuBarItem->isHighlighted());
    QVERIFY(helpMenuBarItem->isHighlighted());
    QVERIFY(helpMenuBarMenu->isVisible());
    QTRY_VERIFY(!viewMenuBarMenu->isVisible());
    QTRY_VERIFY(!alignmentSubMenu->isVisible());
    QTRY_VERIFY(!horizontalSubMenu->isVisible());
    QTRY_VERIFY(helpMenuBarMenu->isOpened());

    // navigate back
    QTest::keyClick(window.data(), Qt::Key_Left);
    QVERIFY(!helpMenuBarItem->isHighlighted());
    QVERIFY(viewMenuBarItem->isHighlighted());
    QVERIFY(viewMenuBarMenu->isVisible());
    QTRY_VERIFY(!helpMenuBarMenu->isVisible());
    QTRY_VERIFY(viewMenuBarMenu->isOpened());

    // re-open the chain of menus
    QTest::keyClick(window.data(), Qt::Key_Down);
    QVERIFY(alignmentSubMenuItem->isHighlighted());
    QTest::keyClick(window.data(), Qt::Key_Right);
    QTRY_VERIFY(alignmentSubMenu->isOpened());
    QTest::keyClick(window.data(), Qt::Key_Right);
    QTRY_VERIFY(horizontalSubMenu->isOpened());

    // repeat escape to close the whole chain of menus one by one
    QTest::keyClick(window.data(), Qt::Key_Escape);
    QTRY_VERIFY(!horizontalSubMenu->isVisible());
    QVERIFY(viewMenuBarItem->isHighlighted());
    QVERIFY(viewMenuBarMenu->isVisible());
    QVERIFY(alignmentSubMenu->isVisible());

    QTest::keyClick(window.data(), Qt::Key_Escape);
    QTRY_VERIFY(!alignmentSubMenu->isVisible());
    QVERIFY(viewMenuBarItem->isHighlighted());
    QVERIFY(viewMenuBarMenu->isVisible());

    QTest::keyClick(window.data(), Qt::Key_Escape);
    QVERIFY(!viewMenuBarItem->isHighlighted());
    QTRY_VERIFY(!viewMenuBarMenu->isVisible());
}

void tst_qquickmenubar::mnemonics()
{
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);
    if (!hasWindowActivation())
        QSKIP("Window activation is not supported");

#if defined(Q_OS_MACOS) or defined(Q_OS_WEBOS)
    QSKIP("Mnemonics are not used on this platform");
#endif

    QQmlApplicationEngine engine(testFileUrl("menubaritems.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);

    centerOnScreen(window.data());
    moveMouseAway(window.data());
    QVERIFY(QTest::qWaitForWindowActive(window.data()));

    MnemonicKeySimulator keySim(window.data());

    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    QQuickMenu *fileMenuBarMenu = menuBar->menuAt(0);
    QQuickMenu *editMenuBarMenu = menuBar->menuAt(1);
    QQuickMenu *viewMenuBarMenu = menuBar->menuAt(2);
    QQuickMenu *helpMenuBarMenu = menuBar->menuAt(3);
    QVERIFY(fileMenuBarMenu && editMenuBarMenu && viewMenuBarMenu && helpMenuBarMenu);

    QQuickMenuBarItem *fileMenuBarItem = qobject_cast<QQuickMenuBarItem *>(fileMenuBarMenu->parentItem());
    QQuickMenuBarItem *editMenuBarItem = qobject_cast<QQuickMenuBarItem *>(editMenuBarMenu->parentItem());
    QQuickMenuBarItem *viewMenuBarItem = qobject_cast<QQuickMenuBarItem *>(viewMenuBarMenu->parentItem());
    QQuickMenuBarItem *helpMenuBarItem = qobject_cast<QQuickMenuBarItem *>(helpMenuBarMenu->parentItem());
    QVERIFY(fileMenuBarItem && editMenuBarItem && viewMenuBarItem && helpMenuBarItem);

    QQuickButton *oopsButton = window->property("oopsButton").value<QQuickButton *>();
    QVERIFY(oopsButton);
    QSignalSpy oopsButtonSpy(oopsButton, &QQuickButton::clicked);
    QVERIFY(oopsButtonSpy.isValid());

    // trigger a menubar item to open a menu
    keySim.press(Qt::Key_Alt);
    keySim.click(Qt::Key_E); // "&Edit"
    keySim.release(Qt::Key_Alt);
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(!editMenuBarItem->hasActiveFocus());
    QVERIFY(editMenuBarMenu->isVisible());
    QTRY_VERIFY(editMenuBarMenu->isOpened());
    QVERIFY(editMenuBarMenu->hasActiveFocus());

    // press Alt to hide the menu
    keySim.click(Qt::Key_Alt);
    QVERIFY(!editMenuBarItem->isHighlighted());
    QVERIFY(!editMenuBarItem->hasActiveFocus());
    QVERIFY(!editMenuBarMenu->hasActiveFocus());
    QTRY_VERIFY(!editMenuBarMenu->isVisible());

    // re-trigger a menubar item to show the menu again
    keySim.press(Qt::Key_Alt);
    keySim.click(Qt::Key_E); // "&Edit"
    keySim.release(Qt::Key_Alt);
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(editMenuBarMenu->isVisible());
    QTRY_VERIFY(editMenuBarMenu->isOpened());
    QVERIFY(editMenuBarMenu->hasActiveFocus());
    QVERIFY(!editMenuBarItem->hasActiveFocus());

    // trigger another menubar item to open another menu, leave Alt pressed
    keySim.press(Qt::Key_Alt);
    QTRY_VERIFY(!editMenuBarMenu->isVisible());
    keySim.click(Qt::Key_H); // "&Help"
    QVERIFY(!editMenuBarItem->isHighlighted());
    QVERIFY(helpMenuBarItem->isHighlighted());
    QVERIFY(!viewMenuBarMenu->isVisible());
    QVERIFY(helpMenuBarMenu->isVisible());
    QTRY_VERIFY(helpMenuBarMenu->isOpened());

    // trigger a menu item to close the menu
    keySim.click(Qt::Key_A); // "&About"
    keySim.release(Qt::Key_Alt);
    QVERIFY(!helpMenuBarItem->isHighlighted());
    QTRY_VERIFY(!helpMenuBarMenu->isVisible());

    // trigger a menubar item to open a menu, leave Alt pressed
    keySim.press(Qt::Key_Alt);
    keySim.click(Qt::Key_V); // "&View"
    QVERIFY(!editMenuBarItem->isHighlighted());
    QVERIFY(viewMenuBarItem->isHighlighted());
    QVERIFY(viewMenuBarMenu->isVisible());
    QTRY_VERIFY(viewMenuBarMenu->isOpened());

    // trigger a menu item to open a sub-menu, leave Alt pressed
    QQuickMenuItem *alignmentSubMenuItem = qobject_cast<QQuickMenuItem *>(viewMenuBarMenu->itemAt(0));
    QVERIFY(alignmentSubMenuItem);
    QQuickMenu *alignmentSubMenu = alignmentSubMenuItem->subMenu();
    QVERIFY(alignmentSubMenu);
    keySim.click(Qt::Key_A); // "&Alignment"
#ifndef Q_OS_ANDROID
    // On Android sub-menus are not cascading, so the Alignment sub-menu is
    // shown instead of View menu.
    QVERIFY(viewMenuBarMenu->isVisible());
#endif
    QVERIFY(alignmentSubMenu->isVisible());
    QTRY_VERIFY(alignmentSubMenu->isOpened());

    // trigger a menu item to open a sub-sub-menu, leave Alt pressed
    QQuickMenuItem *verticalSubMenuItem = qobject_cast<QQuickMenuItem *>(alignmentSubMenu->itemAt(1));
    QVERIFY(verticalSubMenuItem);
    QQuickMenu *verticalSubMenu = verticalSubMenuItem->subMenu();
    QVERIFY(verticalSubMenu);
    keySim.click(Qt::Key_V); // "&Vertical"
#ifndef Q_OS_ANDROID
    // On Android sub-menus are not cascading, so the Vertical sub-menu is
    // shown instead of View menu and Alignment sub-menu.
    QVERIFY(viewMenuBarMenu->isVisible());
    QVERIFY(alignmentSubMenu->isVisible());
#endif
    QVERIFY(verticalSubMenu->isVisible());
    QTRY_VERIFY(verticalSubMenu->isOpened());

    // trigger a menu item to close the whole chain of menus
    keySim.click(Qt::Key_C); // "&Center"
    keySim.release(Qt::Key_Alt);
    QVERIFY(!viewMenuBarItem->isHighlighted());
    QTRY_VERIFY(!viewMenuBarMenu->isVisible());
    QTRY_VERIFY(!alignmentSubMenu->isVisible());
    QTRY_VERIFY(!verticalSubMenu->isVisible());

    // trigger a menubar item to open a menu, leave Alt pressed
    keySim.press(Qt::Key_Alt);
    keySim.click(Qt::Key_F); // "&File"
    QVERIFY(fileMenuBarItem->isHighlighted());
    QVERIFY(fileMenuBarMenu->isVisible());
    QTRY_VERIFY(fileMenuBarMenu->isOpened());
    QVERIFY(fileMenuBarMenu->hasActiveFocus());

    // trigger a menu item to close the menu, which shouldn't trigger a button
    // action behind the menu (QTBUG-86276)
    QCOMPARE(oopsButtonSpy.size(), 0);
    keySim.click(Qt::Key_O); // "&Open..."
    keySim.release(Qt::Key_Alt);
    QVERIFY(!fileMenuBarItem->isHighlighted());
    QVERIFY(!fileMenuBarMenu->isOpened());
    QTRY_VERIFY(!fileMenuBarMenu->isVisible());
    QCOMPARE(oopsButtonSpy.size(), 0);

    // trigger a button action while menu is closed
    keySim.press(Qt::Key_Alt);
    keySim.click(Qt::Key_O); // "&Oops"
    keySim.release(Qt::Key_Alt);
    QCOMPARE(oopsButtonSpy.size(), 1);
}

void tst_qquickmenubar::altNavigation()
{
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);
    if (!QGuiApplicationPrivate::platformTheme()->themeHint(QPlatformTheme::MenuBarFocusOnAltPressRelease).toBool())
        QSKIP("Menu doesn't get focus via Alt press&release on this platform");

    QQmlApplicationEngine engine(testFileUrl("menubaritems.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);

    centerOnScreen(window.data());
    moveMouseAway(window.data());
    QVERIFY(QTest::qWaitForWindowActive(window.data()));

    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    QQuickMenu *fileMenuBarMenu = menuBar->menuAt(0);
    QQuickMenuBarItem *fileMenuBarItem = qobject_cast<QQuickMenuBarItem *>(fileMenuBarMenu->parentItem());

    // This logic is somewhat inverted, but QKeyEvent::modifiers() XOR's the modifier
    // corresponding to the activated key, so pressing Alt adds the AltModifier, and
    // releasing Alt with AltModifier removes the AltModifier.
    QTest::keyPress(window.get(), Qt::Key_Alt);
    QTest::keyRelease(window.get(), Qt::Key_Alt, Qt::AltModifier);
    QVERIFY(menuBar->hasActiveFocus());
    QVERIFY(fileMenuBarItem->isHighlighted());

    // if menu has focus, pressing the mnemonic without Alt should open the menu
    QQuickMenu *editMenuBarMenu = menuBar->menuAt(1);
    QQuickMenuBarItem *editMenuBarItem = qobject_cast<QQuickMenuBarItem *>(editMenuBarMenu->parentItem());

    QTest::keyPress(window.get(), Qt::Key_E);
    QVERIFY(editMenuBarItem->isHighlighted());
    QVERIFY(editMenuBarMenu->isVisible());
    QTRY_VERIFY(editMenuBarMenu->isOpened());
    QVERIFY(editMenuBarMenu->hasActiveFocus());
}

void tst_qquickmenubar::addRemove_data()
{
    QTest::addColumn<QString>("testUrl");
    QTest::addColumn<bool>("native");
    QTest::newRow("menuitems, not native") << QStringLiteral("empty.qml") << false;
    if (nativeMenuBarSupported)
        QTest::newRow("menuitems, native") << QStringLiteral("empty.qml") << true;
}

void tst_qquickmenubar::addRemove()
{
    QFETCH(QString, testUrl);
    QFETCH(bool, native);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, !native);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl(testUrl));

    QQuickMenuBar *menuBar = qobject_cast<QQuickMenuBar *>(engine.rootObjects().value(0));
    QVERIFY(menuBar);
    QQuickMenuBarPrivate *menuBarPrivate = QQuickMenuBarPrivate::get(menuBar);
    QCOMPARE(menuBarPrivate->useNativeMenuBar(), native);
    if (native)
        QVERIFY(menuBarPrivate->nativeHandle());

    QQmlComponent component(&engine);
    component.setData("import QtQuick.Controls; Menu { }", QUrl());

    QPointer<QQuickMenu> menu1(qobject_cast<QQuickMenu *>(component.create()));
    QVERIFY(!menu1.isNull());
    menuBar->addMenu(menu1.data());
    QCOMPARE(menuBar->count(), 1);
    QCOMPARE(menuBar->menuAt(0), menu1.data());

    QPointer<QQuickMenuBarItem> menuBarItem1(qobject_cast<QQuickMenuBarItem *>(menuBar->itemAt(0)));
    QVERIFY(menuBarItem1);
    QCOMPARE(menuBarItem1->menu(), menu1.data());
    QCOMPARE(menuBar->itemAt(0), menuBarItem1.data());

    QPointer<QQuickMenu> menu2(qobject_cast<QQuickMenu *>(component.create()));
    QVERIFY(!menu2.isNull());
    menuBar->insertMenu(0, menu2.data());
    QCOMPARE(menuBar->count(), 2);
    QCOMPARE(menuBar->menuAt(0), menu2.data());
    QCOMPARE(menuBar->menuAt(1), menu1.data());

    QPointer<QQuickMenuBarItem> menuBarItem2(qobject_cast<QQuickMenuBarItem *>(menuBar->itemAt(0)));
    QVERIFY(menuBarItem2);
    QCOMPARE(menuBarItem2->menu(), menu2.data());
    QCOMPARE(menuBar->itemAt(0), menuBarItem2.data());
    QCOMPARE(menuBar->itemAt(1), menuBarItem1.data());

    // takeMenu(int) does not explicitly destroy the menu, but leave
    // this to the garbage collector. The MenuBarItem, OTOH, is currently
    // being destroyed from c++, but this might change in the future.
    QCOMPARE(menuBar->takeMenu(1), menu1.data());
    QCOMPARE(menuBar->count(), 1);
    QVERIFY(!menuBar->menuAt(1));
    QVERIFY(!menuBar->itemAt(1));
    QTRY_VERIFY(menuBarItem1.isNull());
    QVERIFY(!menu1.isNull());
    gc(engine);
    QVERIFY(!menu1.isNull());

    // check that it's safe to call takeMenu(int) with
    // an index that is out of range.
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(".*out of range"));
    QCOMPARE(menuBar->takeMenu(-1), nullptr);
    QCOMPARE(menuBar->count(), 1);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(".*out of range"));
    QCOMPARE(menuBar->takeMenu(10), nullptr);
    QCOMPARE(menuBar->count(), 1);

    // addMenu(Menu) re-creates the respective item in the menubar
    menuBar->addMenu(menu1.data());
    QCOMPARE(menuBar->count(), 2);
    menuBarItem1 = qobject_cast<QQuickMenuBarItem *>(menuBar->itemAt(1));
    QVERIFY(!menuBarItem1.isNull());

    // removeMenu(menu) does not explicitly destroy the menu, but leave
    // this to the garbage collector. The MenuBarItem, OTOH, is currently
    // being destroyed from c++, but this might change in the future.
    menuBar->removeMenu(menu1.data());
    QCOMPARE(menuBar->count(), 1);
    QVERIFY(!menuBar->itemAt(1));
    QTRY_VERIFY(menuBarItem1.isNull());
    QVERIFY(!menu1.isNull());
    gc(engine);
    QVERIFY(!menu1.isNull());
}

void tst_qquickmenubar::addRemoveInlineMenus_data()
{
    QTest::addColumn<bool>("native");
    QTest::newRow("not native") << false;
    if (nativeMenuBarSupported)
        QTest::newRow("native") << true;
}

void tst_qquickmenubar::addRemoveInlineMenus()
{
    // Check that it's safe to remove a menu from the menubar, that
    // is an inline child from QML (fileMenu). Since it's owned by
    // JavaScript, it should be deleted by the gc when appropriate, and
    // not upon a call to removeMenu.
    QFETCH(bool, native);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, !native);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl("menus.qml"));

    auto window = qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0));
    QVERIFY(window);
    auto menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    QPointer<QQuickMenu> fileMenu = window->property("fileMenu").value<QQuickMenu *>();
    QVERIFY(fileMenu);
    QCOMPARE(menuBar->menuAt(0), fileMenu);

    QPointer<QQuickItem> menuBarItem = menuBar->itemAt(0);
    QVERIFY(menuBarItem);

    menuBar->removeMenu(fileMenu);
    QVERIFY(menuBar->menuAt(0) != fileMenu);
    QTRY_VERIFY(!menuBarItem);
    QVERIFY(fileMenu);
    gc(engine);
    QVERIFY(fileMenu);

    // Add it back again, but to the end. This should also be fine, even
    // if it no longer matches the initial order in the QML file.
    menuBar->addMenu(fileMenu);
    QVERIFY(fileMenu);
    QCOMPARE(menuBar->menuAt(menuBar->count() - 1), fileMenu);
}

void tst_qquickmenubar::addRemoveMenuFromQml_data()
{
    QTest::addColumn<bool>("native");
    QTest::newRow("not native") << false;
    if (nativeMenuBarSupported)
        QTest::newRow("native") << true;
}

void tst_qquickmenubar::addRemoveMenuFromQml()
{
    // Create a menu dynamically from QML, and add it to
    // the menubar. Remove it again. Check that the
    // garbage collector will then destruct it.
    QFETCH(bool, native);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, !native);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl("menus.qml"));

    auto window = qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0));
    QVERIFY(window);
    auto menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    const int initialMenuCount = menuBar->count();
    QVERIFY(initialMenuCount > 0);

    QMetaObject::invokeMethod(window, "addTestMenu");

    QCOMPARE(menuBar->count(), initialMenuCount + 1);

    // The "extra" menu should have been added to
    // the end of the menu bar. Verify this.
    QQuickItem *item = menuBar->itemAt(menuBar->count() - 1);
    QPointer<QQuickMenuBarItem> menuBarItem = qobject_cast<QQuickMenuBarItem *>(item);
    QVERIFY(menuBarItem);
    QPointer<QQuickMenu> menu = menuBar->menuAt(menuBar->count() - 1);
    QVERIFY(menu);
    QCOMPARE(menu->title(), "extra");
    QCOMPARE(menuBarItem->menu(), menu);

    // Remove the menu again. Since we have no other references to
    // it from QML, it should be collected by the gc.
    menuBar->removeMenu(menu);
    QCOMPARE(menuBar->count(), initialMenuCount);
    QTRY_VERIFY(!menuBarItem);
    QVERIFY(menu);
    gc(engine);
    QVERIFY(!menu);
}

void tst_qquickmenubar::insert_data()
{
    QTest::addColumn<bool>("native");
    QTest::newRow("not native") << false;
    if (nativeMenuBarSupported)
        QTest::newRow("native") << true;
}

void tst_qquickmenubar::insert()
{
    QFETCH(bool, native);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, !native);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl("menus.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);
    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    const int initialMenuCount = menuBar->count();
    QVERIFY(initialMenuCount > 0);

    QQmlComponent component(&engine);
    component.setData("import QtQuick.Controls; Menu { }", QUrl());

    QPointer<QQuickMenu> menu1(qobject_cast<QQuickMenu *>(component.create()));
    QVERIFY(!menu1.isNull());
    menuBar->insertMenu(0, menu1.data());
    QCOMPARE(menuBar->count(), initialMenuCount + 1);
    QCOMPARE(menuBar->menuAt(0), menu1.data());

    QPointer<QQuickMenu> menu2(qobject_cast<QQuickMenu *>(component.create()));
    QVERIFY(!menu2.isNull());
    menuBar->insertMenu(2, menu2.data());
    QCOMPARE(menuBar->count(), initialMenuCount + 2);
    QCOMPARE(menuBar->menuAt(2), menu2.data());
}

void tst_qquickmenubar::removeMenuThatIsOpen()
{
    // Check that if we remove a menu that is open, it ends
    // up being hidden / closed. This is mostly important for
    // non-native menubars.
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl("menus.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);
    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    QQuickMenu *fileMenu = window->property("fileMenu").value<QQuickMenu *>();
    QVERIFY(fileMenu);
    fileMenu->open();
    QVERIFY(fileMenu->isVisible());
    menuBar->removeMenu(fileMenu);
    QVERIFY(fileMenu);
    QTRY_VERIFY(!fileMenu->isVisible());
}

void tst_qquickmenubar::addRemoveExistingMenus_data()
{
    QTest::addColumn<bool>("native");
    QTest::newRow("not native") << false;
    if (nativeMenuBarSupported)
        QTest::newRow("native") << true;
}

void tst_qquickmenubar::addRemoveExistingMenus()
{
    // Check that you get warnings if trying to add menus that
    // are already in the menubar, or remove menus that are not.
    QFETCH(bool, native);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, !native);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl("menus.qml"));

    auto window = qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0));
    QVERIFY(window);
    auto menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    QPointer<QQuickMenu> fileMenu = window->property("fileMenu").value<QQuickMenu *>();
    QVERIFY(fileMenu);
    QCOMPARE(menuBar->menuAt(0), fileMenu);

    QTest::ignoreMessage(QtWarningMsg, QRegularExpression("cannot add menu.*"));
    menuBar->addMenu(fileMenu);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression("cannot insert menu.*"));
    menuBar->insertMenu(0, fileMenu);
    menuBar->removeMenu(fileMenu);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression("cannot remove menu.*"));
    menuBar->removeMenu(fileMenu);
}

void tst_qquickmenubar::checkHighlightWhenMenuDismissed()
{
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);
    if ((QGuiApplication::platformName() == QLatin1String("offscreen"))
        || (QGuiApplication::platformName() == QLatin1String("minimal")))
        QSKIP("Mouse highlight not functional on offscreen/minimal platforms");

    QQmlApplicationEngine engine(testFileUrl("checkHighlightWhenDismissed.qml"));
    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);

    centerOnScreen(window.data());
    moveMouseAway(window.data());
    QVERIFY(QTest::qWaitForWindowExposed(window.data()));

    QQuickMenuBar *menuBar = window->findChild<QQuickMenuBar *>("menuBar");
    QVERIFY(menuBar);

    QQuickMenu *staticMenu = menuBar->menuAt(0);
    QQuickMenu *dynamicMenu = menuBar->menuAt(1);
    QVERIFY(staticMenu && dynamicMenu);
    QQuickMenuBarItem *staticMenuBarItem = qobject_cast<QQuickMenuBarItem *>(staticMenu->parentItem());
    QQuickMenuBarItem *dynamicMenuBarItem = qobject_cast<QQuickMenuBarItem *>(dynamicMenu->parentItem());
    QVERIFY(staticMenuBarItem && dynamicMenuBarItem);

    // highlight the static MenuBarItem and open the menu
    QTest::mouseMove(window.data(), staticMenuBarItem->mapToScene(
        QPointF(staticMenuBarItem->width() / 2, staticMenuBarItem->height() / 2)).toPoint());
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier,
        staticMenuBarItem->mapToScene(QPointF(staticMenuBarItem->width() / 2, staticMenuBarItem->height() / 2)).toPoint());
    QVERIFY(staticMenuBarItem->isHighlighted());
    QVERIFY(staticMenu->isVisible());
    QTRY_VERIFY(staticMenu->isOpened());
    // click a menu item to dismiss the menu and unhighlight the static MenuBarItem
    QQuickMenuItem *menuItem = qobject_cast<QQuickMenuItem *>(staticMenu->itemAt(0));
    QVERIFY(menuItem);
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier,
        menuItem->mapToScene(QPointF(menuItem->width() / 2, menuItem->height() / 2)).toPoint());
    QVERIFY(!staticMenuBarItem->isHighlighted());
    // wait for the menu to be fully gone so that it doesn't interfere with the next test
    QTRY_VERIFY(!staticMenu->isVisible());

    // highlight the dynamic MenuBarItem and open the menu
    QTest::mouseMove(window.data(), dynamicMenuBarItem->mapToScene(
        QPointF(dynamicMenuBarItem->width() / 2, dynamicMenuBarItem->height() / 2)).toPoint());
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier,
        dynamicMenuBarItem->mapToScene(QPointF(dynamicMenuBarItem->width() / 2, dynamicMenuBarItem->height() / 2)).toPoint());
    QVERIFY(dynamicMenuBarItem->isHighlighted());
    QVERIFY(dynamicMenu->isVisible());
    QTRY_VERIFY(dynamicMenu->isOpened());

    // click a menu item to dismiss the menu and unhighlight the dynamic MenuBarItem
    menuItem = qobject_cast<QQuickMenuItem *>(dynamicMenu->itemAt(0));
    QVERIFY(menuItem);
    QTest::mouseClick(window.data(), Qt::LeftButton, Qt::NoModifier,
        menuItem->mapToScene(QPointF(menuItem->width() / 2, menuItem->height() / 2)).toPoint());
    QVERIFY(!dynamicMenuBarItem->isHighlighted());
}

void tst_qquickmenubar::hoverAfterClosingWithEscape()
{
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);
    if ((QGuiApplication::platformName() == QLatin1String("offscreen"))
        || (QGuiApplication::platformName() == QLatin1String("minimal")))
        QSKIP("Mouse highlight not functional on offscreen/minimal platforms");

    QQuickControlsApplicationHelper helper(this, QLatin1String("hoverAfterClosingWithEscape.qml"));
    QVERIFY2(helper.ready, helper.failureMessage());
    QQuickApplicationWindow *window = helper.appWindow;
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    QQuickMenuBar *menuBar = window->findChild<QQuickMenuBar *>("menuBar");
    QVERIFY(menuBar);

    // Open the first menu by clicking on the first menu bar item.
    auto *firstMenuBarItem(qobject_cast<QQuickMenuBarItem *>(menuBar->itemAt(0)));
    QVERIFY(clickButton(firstMenuBarItem));
    QQuickMenu *firstMenu = menuBar->menuAt(0);
    QVERIFY(firstMenu);
    QTRY_VERIFY(firstMenu->isOpened());

    // Close it with the escape key.
    QTest::keyClick(window, Qt::Key_Escape);
    QTRY_VERIFY(!firstMenu->isVisible());

    // Hover over the second menu bar item; it shouldn't cause its menu to open.
    auto *secondMenuBarItem(qobject_cast<QQuickMenuBarItem *>(menuBar->itemAt(1)));
    QTest::mouseMove(window, mapCenterToWindow(secondMenuBarItem));
    QQuickMenu *secondMenu = menuBar->menuAt(1);
    QVERIFY(secondMenu);
    QVERIFY(!secondMenu->isVisible());
}

void tst_qquickmenubar::AA_DontUseNativeMenuBar()
{
    // Check that we end up with a non-native menu bar when AA_DontUseNativeMenuBar is set.
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl("menus.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);
    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);
    auto menuBarPrivate = QQuickMenuBarPrivate::get(menuBar);
    QQuickItem *contents = window->property("contents").value<QQuickItem *>();
    QVERIFY(contents);

    QVERIFY(!menuBarPrivate->nativeHandle());
    QVERIFY(menuBar->isVisible());
    QVERIFY(menuBar->count() > 0);
    QVERIFY(menuBar->height() > 0);
    QCOMPARE(contents->height(), window->height() - menuBar->height());

    // If the menu bar is not native, the menus should not be native either.
    // The main reason for this limitation is that a native menu typically
    // run in separate native event loop which will not forward mouse events
    // to Qt. And this is needed for a non-native menu bar to work (e.g to
    // support hovering over the menu bar items to open and close menus).
    const auto firstMenu = menuBar->menuAt(0);
    QVERIFY(firstMenu);
    QVERIFY(!QQuickMenuPrivate::get(firstMenu)->maybeNativeHandle());
}

void tst_qquickmenubar::containerItems_data()
{
    QTest::addColumn<QString>("testUrl");
    QTest::addColumn<bool>("native");
    QTest::newRow("menuitems, not native") << QStringLiteral("menubaritems.qml") << false;
    QTest::newRow("menus, not native") << QStringLiteral("menus.qml") << false;
    if (nativeMenuBarSupported) {
        QTest::newRow("menuitems, native") << QStringLiteral("menubaritems.qml") << true;
        QTest::newRow("menus, native") << QStringLiteral("menus.qml") << true;
    }
}

void tst_qquickmenubar::containerItems()
{
    // Check that the MenuBar ends up containing a MenuBarItem
    // for each Menu added. This should be the case regardless of
    // if the MenuBar is native or not. There are several ways
    // of accessing those MenuBarItems and menus in the MenuBar
    // API, so check that all end up in sync.
    QFETCH(QString, testUrl);
    QFETCH(bool, native);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, !native);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl(testUrl));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);
    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);
    auto *menuBarPrivate = QQuickMenuBarPrivate::get(menuBar);
    QCOMPARE(menuBarPrivate->useNativeMenuBar(), native);

    QCOMPARE(menuBar->count(), 4);
    for (int i = 0; i < menuBar->count(); ++i) {
        QQuickMenu *menu = menuBar->menuAt(i);
        QVERIFY(menu);

        // Test the itemAt() API
        QQuickItem *item = menuBar->itemAt(i);
        QVERIFY(item);
        auto menuBarItem = qobject_cast<QQuickMenuBarItem *>(item);
        QVERIFY(menuBarItem);
        QCOMPARE(menuBarItem->menu(), menu);

        // Test the "contentData" list property API
        auto cd = menuBarPrivate->contentData();
        QCOMPARE(cd.count(&cd), menuBar->count());
        auto cdItem = static_cast<QQuickItem *>(cd.at(&cd, i));
        QVERIFY(cdItem);
        auto cdMenuBarItem = qobject_cast<QQuickMenuBarItem *>(cdItem);
        QVERIFY(cdMenuBarItem);
        QCOMPARE(cdMenuBarItem->menu(), menu);

        // Test the "menus" list property API
        auto menus = QQuickMenuBarPrivate::get(menuBar)->menus();
        QCOMPARE(menus.count(&menus), menuBar->count());
        auto menusMenu = menus.at(&menus, i);
        QVERIFY(menusMenu);
        QCOMPARE(menusMenu, menu);
    }
}

void tst_qquickmenubar::mixedContainerItems_data()
{
    QTest::addColumn<bool>("native");
    QTest::newRow("not native") << false;
    if (nativeMenuBarSupported)
        QTest::newRow("native") << true;
}

void tst_qquickmenubar::mixedContainerItems()
{
    // The application is allowed to add items other
    // than MenuBarItems and Menus as children. But those
    // should just be ignored by the MenuBar (and the Container).
    QFETCH(bool, native);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, !native);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl("mixed.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);
    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);

    // The menubar has four children, but only three of them are
    // Menus and MenuBarItems. So we should therefore only end up
    // with three menus in the MenuBar, and three items in the Container.
    QCOMPARE(menuBar->count(), 3);
    for (int i = 0; i < 3; ++i) {
        auto item = menuBar->itemAt(i);
        QVERIFY(item);
        auto menuBarItem = qobject_cast<QQuickMenuBarItem *>(item);
        QVERIFY(menuBarItem);
        QCOMPARE(menuBarItem->menu(), menuBar->menuAt(i));
    }

    // Try to add an unsupported item dynamically. It should
    // have no impact on the MenuBar/Container API.
    QQmlComponent component(&engine);
    component.setData("import QtQuick; Item { }", QUrl());
    QPointer<QQuickItem> plainItem(qobject_cast<QQuickItem *>(component.create()));
    QVERIFY(plainItem);

    menuBar->addItem(plainItem);
    QCOMPARE(menuBar->count(), 3);
    for (int i = 0; i < 3; ++i) {
        auto item = menuBar->itemAt(i);
        QVERIFY(item);
        auto menuBarItem = qobject_cast<QQuickMenuBarItem *>(item);
        QVERIFY(menuBarItem);
        QCOMPARE(menuBarItem->menu(), menuBar->menuAt(i));
    }

    // Remove it again. It should have no impact on
    // the MenuBar/Container API.
    menuBar->removeItem(plainItem);
    QCOMPARE(menuBar->count(), 3);
    for (int i = 0; i < 3; ++i) {
        auto item = menuBar->itemAt(i);
        QVERIFY(item);
        auto menuBarItem = qobject_cast<QQuickMenuBarItem *>(item);
        QVERIFY(menuBarItem);
        QCOMPARE(menuBarItem->menu(), menuBar->menuAt(i));
    }
}

void tst_qquickmenubar::applicationWindow_data()
{
    QTest::addColumn<bool>("initiallyNative");
    QTest::addColumn<bool>("initiallyVisible");
    QTest::newRow("initially not native, visible") << false << true;
    QTest::newRow("initially not native, hidden") << false << false;
    if (nativeMenuBarSupported) {
        QTest::newRow("initially native, visible") << true << true;
        QTest::newRow("initially native, hidden") << true << false;
    }
}

void tst_qquickmenubar::applicationWindow()
{
    // Check that ApplicationWindow adds or removes the non-native
    // menubar in response to toggling Qt::AA_DontUseNativeMenuBar and
    // MenuBar.visible.
    QFETCH(bool, initiallyNative);
    QFETCH(bool, initiallyVisible);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, !initiallyNative);
    QQmlApplicationEngine engine;
    engine.setInitialProperties({{ "visible", initiallyVisible }});
    engine.load(testFileUrl("menus.qml"));

    QPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);
    QQuickMenuBar *menuBar = window->property("menuBar").value<QQuickMenuBar *>();
    QVERIFY(menuBar);
    auto menuBarPrivate = QQuickMenuBarPrivate::get(menuBar);
    QQuickItem *contents = window->property("contents").value<QQuickItem *>();
    QVERIFY(contents);

    for (const bool visible : {initiallyVisible, !initiallyVisible, initiallyVisible}) {
        menuBar->setVisible(visible);

        const bool nativeMenuBarVisible = bool(menuBarPrivate->nativeHandle());
        QCOMPARE(nativeMenuBarVisible, initiallyNative && visible);

        if (!visible) {
            QVERIFY(!menuBar->isVisible());
            QVERIFY(!nativeMenuBarVisible);
            QCOMPARE(contents->height(), window->height());
        } else if (nativeMenuBarVisible) {
            QVERIFY(menuBar->isVisible());
            QCOMPARE(contents->height(), window->height());
        } else {
            QVERIFY(menuBar->isVisible());
            QVERIFY(menuBar->height() > 0);
            QCOMPARE(contents->height(), window->height() - menuBar->height());
        }
    }
}

void tst_qquickmenubar::menubarAsHeader_data()
{
    QTest::addColumn<bool>("native");
    QTest::newRow("not native") << false;
    if (nativeMenuBarSupported)
        QTest::newRow("native") << true;
}

void tst_qquickmenubar::menubarAsHeader()
{
    // ApplicationWindow.menuBar was added in Qt 5.10. Before that
    // the menuBar was supposed to be assigned to ApplicationWindow.header.
    // For backwards compatibility, check that you can still do that.
    QFETCH(bool, native);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, !native);
    QQmlApplicationEngine engine;
    engine.load(testFileUrl("menubarAsHeader.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(engine.rootObjects().value(0)));
    QVERIFY(window);
    QQuickMenuBar *menuBar = window->property("header").value<QQuickMenuBar *>();
    QVERIFY(menuBar);
    auto menuBarPrivate = QQuickMenuBarPrivate::get(menuBar);
    QQuickItem *contents = window->property("contents").value<QQuickItem *>();
    QVERIFY(contents);
    QVERIFY(menuBar->count() > 0);
    QCOMPARE(menuBarPrivate->nativeHandle() != nullptr, native);

    if (menuBarPrivate->nativeHandle()) {
        // Using native menubar
        QCOMPARE(contents->height(), window->height());
    } else {
        // Not using native menubar
        QCOMPARE(contents->height(), window->height() - menuBar->height());
    }
}

QTEST_QUICKCONTROLS_MAIN(tst_qquickmenubar)

#include "tst_qquickmenubar.moc"
