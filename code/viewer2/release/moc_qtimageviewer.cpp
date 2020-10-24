/****************************************************************************
** Meta object code from reading C++ file 'qtimageviewer.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qtimageviewer.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtimageviewer.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtImageViewer_t {
    QByteArrayData data[15];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtImageViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtImageViewer_t qt_meta_stringdata_QtImageViewer = {
    {
QT_MOC_LITERAL(0, 0, 13), // "QtImageViewer"
QT_MOC_LITERAL(1, 14, 8), // "showFile"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 8), // "filename"
QT_MOC_LITERAL(4, 33, 9), // "showImage"
QT_MOC_LITERAL(5, 43, 6), // "Image*"
QT_MOC_LITERAL(6, 50, 3), // "img"
QT_MOC_LITERAL(7, 54, 14), // "transformation"
QT_MOC_LITERAL(8, 69, 6), // "float*"
QT_MOC_LITERAL(9, 76, 6), // "values"
QT_MOC_LITERAL(10, 83, 10), // "nrOfValues"
QT_MOC_LITERAL(11, 94, 13), // "showImageLeft"
QT_MOC_LITERAL(12, 108, 14), // "showImageRight"
QT_MOC_LITERAL(13, 123, 8), // "openFile"
QT_MOC_LITERAL(14, 132, 4) // "quit"

    },
    "QtImageViewer\0showFile\0\0filename\0"
    "showImage\0Image*\0img\0transformation\0"
    "float*\0values\0nrOfValues\0showImageLeft\0"
    "showImageRight\0openFile\0quit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtImageViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x0a /* Public */,
       4,    1,   62,    2, 0x0a /* Public */,
       4,    4,   65,    2, 0x0a /* Public */,
       4,    3,   74,    2, 0x2a /* Public | MethodCloned */,
       4,    2,   81,    2, 0x2a /* Public | MethodCloned */,
      11,    1,   86,    2, 0x0a /* Public */,
      12,    1,   89,    2, 0x0a /* Public */,
      13,    0,   92,    2, 0x0a /* Public */,
      14,    0,   93,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5, QMetaType::Int, 0x80000000 | 8, QMetaType::Int,    6,    7,    9,   10,
    QMetaType::Void, 0x80000000 | 5, QMetaType::Int, 0x80000000 | 8,    6,    7,    9,
    QMetaType::Void, 0x80000000 | 5, QMetaType::Int,    6,    7,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QtImageViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QtImageViewer *_t = static_cast<QtImageViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->showImage((*reinterpret_cast< Image*(*)>(_a[1]))); break;
        case 2: _t->showImage((*reinterpret_cast< Image*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< float*(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 3: _t->showImage((*reinterpret_cast< Image*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< float*(*)>(_a[3]))); break;
        case 4: _t->showImage((*reinterpret_cast< Image*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->showImageLeft((*reinterpret_cast< Image*(*)>(_a[1]))); break;
        case 6: _t->showImageRight((*reinterpret_cast< Image*(*)>(_a[1]))); break;
        case 7: _t->openFile(); break;
        case 8: _t->quit(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QtImageViewer::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QtImageViewer.data,
      qt_meta_data_QtImageViewer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *QtImageViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtImageViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtImageViewer.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int QtImageViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
