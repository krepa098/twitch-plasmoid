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
import QtGraphicalEffects 1.0

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.private.twitchplasmoid 1.0 as Twitch

Item {
    Plasmoid.toolTipMainText: "Twitch"
    Plasmoid.switchWidth: 300
    Plasmoid.switchHeight: 400
    
    Twitch.Client {
        id: client

        onSearchChannelFinishedJS: {
            for(var i=0;i<results.length;i++) {
                console.log(results[i].id, results[i].displayName)
            }
        }
        
        onWatchChannelRequested: {
            client.exec("mpv " + url)
        }

        Component.onCompleted: {
            client.model.load(settings)
        }
    }

    Twitch.Settings {
        id: settings
    }
    
    Plasmoid.fullRepresentation: Item {
        Layout.minimumWidth: 300
        Layout.minimumHeight: 400

        PlasmaComponents.PageStack {
            id: pageStack
            clip: true
            anchors.fill: parent
            initialPage: Qt.createComponent("MainPage.qml")
        }
    }
    
    Plasmoid.compactRepresentation: Item {
        id: tray
        Layout.minimumWidth: layout.implicitWidth
        RowLayout {
            id: layout
            anchors.centerIn: parent
            
            PlasmaCore.IconItem {
                id: trayIcon
                source: "twitch-plasma-appicon"
                Layout.preferredHeight: tray.height - 12
                Layout.preferredWidth: tray.height - 12
                
                ColorOverlay {
                    source: trayIcon
                    anchors.fill: parent
                    color: theme.textColor
                }
            }
            
            Rectangle {
                color: "transparent"
                border.color: theme.textColor
                implicitWidth: liveChannelLabel.implicitWidth + 6
                implicitHeight: Math.min(tray.height, liveChannelLabel.implicitHeight + 6)
                radius: 4
                visible: client.model.liveChannelCount > 0
                
                PlasmaComponents.Label {
                    id: liveChannelLabel
                    anchors.centerIn: parent
                    text: client.model.liveChannelCount 
                    color: theme.textColor
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: plasmoid.expanded = !plasmoid.expanded
        }
    }
    
}
