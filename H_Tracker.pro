QT       += core network gui widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = H_Tracker
TEMPLATE = app

UI_DIR = src/GUI
OBJECTS_DIR = obj
MOC_DIR = moc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS\
           _HAS_CXX17=1\
           SAVE_PREDICTION_FILTERPOSITION=1
           SAVE_TRACK_COUNT=1

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += $$files(src/*.cpp, true)

HEADERS += $$files(src/*.h, true)

FORMS += \
        mainwindow.ui \
        src/GUI/PlotDialog.ui
		
allFilesFolders = $$files(src/*, true))
folders = src
for (item, allFilesFolders): contains(item, [^.]*) : folders += $$item
INCLUDEPATH += $$folders

win32:INCLUDEPATH += 'C:/dev/eigen'

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
