/*
Copyright (C) 2018  Paul Kremer

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.1
import QtQuick.Layouts 1.3

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

ColumnLayout {
    RowLayout {
        PlasmaComponents.ToolButton {
            iconName: "back"

            onClicked: pageStack.pop()
        }
        PlasmaExtras.Title { text: qsTr("Settings") }
    }
    
    PlasmaComponents.Switch {
        text: qsTr("Watch in browser")
        checked: settings.watchInBrowser
        onClicked: settings.watchInBrowser = checked
    }
    
    PlasmaComponents.Label {
        text: qsTr("Command to execute on watch")
    }
    
    PlasmaComponents.TextField {
        Layout.fillWidth: true
        text: settings.cmdOnWatch
        enabled: !settings.watchInBrowser
        onTextChanged: settings.cmdOnWatch = text
    }
    
    PlasmaComponents.Label {
        text: qsTr("e.g. mpv %1 \nwhere %1 represents the Twitch URL")
    }
    
    Item { Layout.fillHeight: true }
}
           
