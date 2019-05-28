import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.2
import USBasp.pp 1.0

ApplicationWindow {
    id: mainWindow
    minimumWidth: 720
    minimumHeight: 530
    visible: true
    title: qsTr("USBasp++")

    property bool partInitPending: true

    Material.theme: themeSwitch.checked ? Material.Dark : Material.Light
    Material.accent: themeSwitch.checked ? Material.Purple : Material.DeepPurple

    ConfigParser {
        id: configParser
    }

    /**
     * Perform some initial value setups provided
     * by the settingsProvider singleton.
     */
    Component.onCompleted: {
        // load application settings when window loading is done
        settingsProvider.loadSettings();
        // set the initial theme here to avoid binding loop
        themeSwitch.checked = settingsProvider.darkTheme
        // set initial value of show full hex path switch
        infoBox.setShowFullHexPathSw(settingsProvider.showFullHexPath)
        // set console output preserve switch
        consoleOutput.setPreserveOutputSw(settingsProvider.preserveConsoleOutput)
        // load parts/mcu list
        configParser.configPath = settingsProvider.avrdudeConf
        configParser.loadPartList()
        mcuSelector.partList = configParser.partList
        // set initial part in mcu/part selection combo box
        if(mainWindow.partInitPending) {
            if(settingsProvider.partId !== "none" && mcuSelector.partList) {
                for(var ix=0; ix < mcuSelector.partList.length; ix++) {
                    if(mcuSelector.partList[ix].id === settingsProvider.partId) {
                        mcuSelector.setInitialPartIndex(ix);
                        break;
                    }
                }
            }

            mainWindow.partInitPending = false
        }
        // set the current page index if available
        mainContent.setCurrentIndex(settingsProvider.currentPage)
        // set initial extra command line flags
        infoBox.setInitialExtraFlags(settingsProvider.extraFlags)
    }

    /**
     * Top toolbar
     * Used for navigation purpose across the pages.
     */
    header: ToolBar {
        id: toolbar

        RowLayout {
            id: toolbarLayout
            width: toolbar.width
            height: toolbar.height

            Button {
                id: prevPageNav
                text:  qsTr("Prev")
                leftPadding: 20
                rightPadding: 20
                flat: true
                Material.foreground: "white"

                onClicked: {
                    // rotate if the current page is the first page
                    if(mainContent.currentIndex === 0) {
                        mainContent.setCurrentIndex(
                            mainContent.count - 1
                        )
                    }else {
                        mainContent.decrementCurrentIndex()
                    }
                }
            }

            ToolSeparator {}

            Row {
                Layout.fillWidth: true
                layoutDirection: Qt.LeftToRight
                spacing: 10

                ToolButton {
                    id: avrdudeNav
                    text: qsTr("avrdude")
                    Material.foreground: "white"
                    font.bold: mainContent.currentIndex === 0 ? true : false
                    font.underline: mainContent.currentIndex === 0 ? true : false

                    onClicked: {
                        mainContent.setCurrentIndex(0)
                    }
                }

                ToolButton {
                    id: consoleNab
                    text: qsTr("console")
                    Material.foreground: "white"
                    font.bold: mainContent.currentIndex === 1 ? true : false
                    font.underline: mainContent.currentIndex === 1 ? true : false

                    onClicked: {
                        mainContent.setCurrentIndex(1)
                    }
                }

                ToolButton {
                    id: aboutNav
                    text: qsTr("about")
                    Material.foreground: "white"
                    font.bold: mainContent.currentIndex === 2 ? true : false
                    font.underline: mainContent.currentIndex === 2 ? true : false

                    onClicked: {
                        mainContent.setCurrentIndex(2)
                    }
                }

                ToolButton {
                    id: licenseNav
                    text: qsTr("license")
                    Material.foreground: "white"
                    font.bold: mainContent.currentIndex === 3 ? true : false
                    font.underline: mainContent.currentIndex === 3 ? true : false

                    onClicked: {
                        mainContent.setCurrentIndex(3)
                    }
                }
            }

            Switch {
                id: themeSwitch
                checked: true
                text: qsTr("Dark Theme")
                Material.foreground: "white"

                onCheckedChanged: {
                    settingsProvider.darkTheme = themeSwitch.checked
                }
            }

            ToolSeparator {}

            Button {
                id: nextPageNav
                text: qsTr("Next")
                rightPadding: 20
                leftPadding: 20
                flat: true
                Material.foreground: "white"

                onClicked: {
                    // rotate if current page is the last Page
                    if(mainContent.currentIndex === (mainContent.count - 1)) {
                        mainContent.setCurrentIndex(0)
                    }else {
                        mainContent.incrementCurrentIndex()
                    }
                }
            }
        }
    }

    /**
     * Main content of the window.
     */
    SwipeView {
        id: mainContent
        width: mainWindow.contentItem.width - 20
        height: mainWindow.contentItem.height - 20
        anchors.centerIn: mainWindow.contentItem
        clip: true
        currentIndex: 0

        onCurrentIndexChanged: {
            if(mainContent.currentIndex === 2) {
                logoAnimation.running = true
            }
            settingsProvider.currentPage = mainContent.currentIndex
        }


        /**
         * Main actions page. Contains all programmer
         * related functionalities including reading, writing
         * and saving data.
         */
        GridLayout {
            width: mainContent.availableWidth
            columns: 3

            McuSelector {
                id: mcuSelector
                Layout.preferredWidth: mainContent.availableWidth/3
                Layout.fillHeight: true

                darkTheme: themeSwitch.checked
                avrdudePath: settingsProvider.avrdudePath

                onSelectedMcuChanged: {
                    if(!mainWindow.partInitPending) {
                        settingsProvider.partId = mcuSelector.selectedMcu.id
                    }
                }
            }
            InputBox {
                Layout.fillWidth: true
                Layout.fillHeight: true

                darkTheme: themeSwitch.checked
            }
            ActionBox {
                Layout.preferredWidth: mainContent.availableWidth/3
                Layout.fillHeight: true

                darkTheme: themeSwitch.checked
            }

            InfoBox {
                id: infoBox
                Layout.columnSpan: 3
                Layout.fillWidth: true
                Layout.fillHeight: false

                darkTheme: themeSwitch.checked

                onShowFullHexPathChanged: {
                    settingsProvider.showFullHexPath = infoBox.showFullHexPath
                }
            }
        }

        /**
         * Console output page.
         * contains terminal/console output redirected from the
         * command line interface.
         */
        ColumnLayout {
            width: mainContent.availableWidth
            height: mainContent.availableHeight
            ConsoleOutput {
                id: consoleOutput
                Layout.fillWidth: true
                Layout.fillHeight: true

                onPreserveOutputChanged: {
                    settingsProvider.preserveConsoleOutput = consoleOutput.preserveOutput
                }
            }
        }

        /**
         * About page
         * Shows credits, version history and logo.
         */
        RowLayout {
            width: mainContent.availableWidth
            height: mainContent.availableHeight

            Image {
                id: appLogo
                source: "qrc:/images/logo.png"

                ParallelAnimation {
                    id: logoAnimation
                    running: false


                    NumberAnimation {
                        target: appLogo
                        property: "scale"
                        duration: 700
                        from: 0.5
                        to: 1.0
                        easing.type: Easing.InOutQuad
                    }


                    NumberAnimation {
                        target: appLogo
                        property: "opacity"
                        duration: 1500
                        easing.type: Easing.InOutQuad
                        from: 0.0
                        to: 1.0
                    }
                }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                TextEdit {
                    readOnly: true
                    text: staticDocuments.aboutText
                    color: themeSwitch.checked? "white": "black"
                    wrapMode: Text.WordWrap
                }
            }
        }

        /**
         * License page.
         * Shows GPL-v2 license in this page.
         */
        ColumnLayout {
            width: mainContent.availableWidth
            height: mainContent.availableHeight
            ScrollView {
                id: licenseView
                Layout.fillWidth: true
                Layout.fillHeight: true

                TextEdit {
                    readOnly: true
                    text: staticDocuments.licenseText
                    color: themeSwitch.checked? "white": "black"
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
