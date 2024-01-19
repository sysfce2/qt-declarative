// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 400
    height: 400

    property alias contextMenu: contextMenu

    Menu {
        id: contextMenu
        objectName: "menu"

        Action {
            objectName: text
            text: "action1"
        }

        MenuSeparator {}

        Menu {
            id: subMenu
            objectName: "subMenu"

            Action {
                objectName: text
                text: "subAction1"
            }

            MenuSeparator {}

            Action {
                objectName: text
                text: "subAction2"
            }
        }
    }
}
