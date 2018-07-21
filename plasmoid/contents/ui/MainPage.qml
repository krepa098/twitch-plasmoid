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
import QtQuick.Controls 1.4 as QQC
import QtQuick.Layouts 1.3

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.private.twitchplasmoid 1.0 as Twitch

PlasmaComponents.Page {
    property bool showSearch: false
    property bool showDelete: false
    
    Item {
        anchors.fill: parent
        anchors.margins: 6

        ColumnLayout {
            anchors.fill: parent
            
            PlasmaExtras.Title { text: "Twitch" }

            RowLayout {
                Layout.fillWidth: true
                
                PlasmaComponents.ToolButton {
                    iconName: "back"

                    visible: showSearch

                    onClicked: {
                        searchBox.text = ""
                    }
                }

                PlasmaComponents.TextField {
                    id: searchBox
                    placeholderText: "Search for a new channel"
                    Layout.fillWidth: true
                    
                    onTextChanged: {
                        if (text === "") {
                            client.searchModel.clear()
                            showSearch = false
                        }
                    }
                    
                    onAccepted: {
                        if (searchBox.text !== "") {
                            client.searchChannel(searchBox.text)
                            showSearch = true
                        }
                    }
                }

                PlasmaComponents.ToolButton {
                    id: searchButton
                    iconName: "search-symbolic"
                    flat: false

                    onClicked: {
                        if (searchBox.text !== "") {
                            client.searchChannel(searchBox.text)
                            showSearch = true
                        }
                    }
                }
                
                PlasmaComponents.ToolButton {
                    iconName: "edit-symbolic"
                    visible: !showSearch
                    flat: true
                    checkable: true

                    onClicked: {
                        showDelete = !showDelete
                    }
                }

            }
            
            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: client.searchInProgress
                
                PlasmaComponents.BusyIndicator {
                    anchors.centerIn: parent
                    width: 64
                    height: 64
                }
            }

            ListView {
                id: searchView
                visible: !client.searchInProgress && showSearch
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: client.searchModel
                spacing: 4
                clip: true
                
                PlasmaComponents.ScrollBar {
                    orientation: Qt.Vertical
                    flickableItem: parent
                }
                
                delegate: RowLayout {
                    width: searchView.width
                    spacing: 10
                    Image {
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 32
                        source: logo
                    }
                    PlasmaComponents.Label { text: displayName }
                    Item { Layout.fillWidth: true }
                    PlasmaComponents.ToolButton {
                        iconName: "list-add"
                        //text: "Add"
                        onClicked: {
                            client.model.addChannelId(channelId, displayName)
                            client.model.save(settings)
                            searchBox.text = ""
                        }
                    }
                }
            }
            
            ListView {
                id: channelView
                visible: !showSearch
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: client.model
                spacing: 4
                clip: true
                
                PlasmaComponents.ScrollBar {
                    id: channelViewScrollBar
                    orientation: Qt.Vertical
                    flickableItem: parent
                }
                
                delegate: Item {
                    width: channelView.width
                    height: 32
                    
                    RowLayout {
                        anchors.fill: parent
                        spacing: 10
                        Item {
                            Layout.preferredWidth: 32
                            Layout.preferredHeight: 32
                            
                            Image {
                                id: channelImage
                                anchors.fill: parent
                                source: logo
                                smooth: true
                                
                                Behavior on opacity { PropertyAnimation {} }
                            }
                            
                            PlasmaCore.IconItem {
                                id: playIcon
                                anchors.fill: parent
                                source: "player-time"
                                opacity: 0
                                
                                Behavior on opacity { PropertyAnimation {} }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: client.exec("mpv " + url)//Qt.openUrlExternally(url)
                                cursorShape: Qt.PointingHandCursor
                                hoverEnabled: true
                                onEntered: {
                                    channelImage.opacity = 0.25
                                    playIcon.opacity = 1
                                }
                                onExited: {
                                    channelImage.opacity = 1
                                    playIcon.opacity = 0
                                }
                            }
                        }
                        
                        PlasmaComponents.Label {
                            Layout.preferredWidth: 124
                            
                            text: displayName
                            font.weight: Font.Bold
                            elide: Text.ElideRight
                        }

                        PlasmaComponents.Label {
                            Layout.fillWidth: true
                            
                            text: game
                            visible: live
                            elide: Text.ElideRight
                        }
                        
                        Item { Layout.fillWidth: true; visible: showDelete }
                        
                        PlasmaComponents.ToolButton {
                            id: deleteButton
                            iconName: "indicator-trashindicator"
                            visible: showDelete
                            flat: true

                            onClicked: {
                                client.model.removeChannel(channelId)
                            }
                        }

                        Rectangle {
                            Layout.minimumWidth: viewersText.implicitWidth + 8
                            Layout.preferredWidth: viewersText.implicitWidth + 8
                            Layout.preferredHeight: viewersText.implicitHeight + 4
                            color: theme.textColor
                            radius: 4
                            visible: !showDelete && live

                            PlasmaComponents.Label {
                                id: viewersText
                                text: live ? viewers : "offline"
                                color: theme.backgroundColor
                                anchors.centerIn: parent
                            }
                        }
                        
                        Item { Layout.preferredWidth: 8; visible: channelViewScrollBar.visible }
                        
                        Item { Layout.fillWidth: true; visible: !showDelete }
                    }
                }
            }

        }
    }
}
