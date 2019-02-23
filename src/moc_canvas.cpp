/****************************************************************************
** Meta object code from reading C++ file 'canvas.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "canvas.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'canvas.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FigureEditor_t {
    QByteArrayData data[5];
    char stringdata0[48];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FigureEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FigureEditor_t qt_meta_stringdata_FigureEditor = {
    {
QT_MOC_LITERAL(0, 0, 12), // "FigureEditor"
QT_MOC_LITERAL(1, 13, 6), // "status"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 12), // "MousePressed"
QT_MOC_LITERAL(4, 34, 13) // "MouseReleased"

    },
    "FigureEditor\0status\0\0MousePressed\0"
    "MouseReleased"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FigureEditor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       3,    0,   32,    2, 0x06 /* Public */,
       4,    0,   33,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FigureEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FigureEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->status((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->MousePressed(); break;
        case 2: _t->MouseReleased(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FigureEditor::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FigureEditor::status)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FigureEditor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FigureEditor::MousePressed)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FigureEditor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FigureEditor::MouseReleased)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FigureEditor::staticMetaObject = { {
    &QGraphicsView::staticMetaObject,
    qt_meta_stringdata_FigureEditor.data,
    qt_meta_data_FigureEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FigureEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FigureEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FigureEditor.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int FigureEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FigureEditor::status(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FigureEditor::MousePressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void FigureEditor::MouseReleased()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_Main_t {
    QByteArrayData data[70];
    char stringdata0[869];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Main_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Main_t qt_meta_stringdata_Main = {
    {
QT_MOC_LITERAL(0, 0, 4), // "Main"
QT_MOC_LITERAL(1, 5, 14), // "SimulationDone"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 11), // "ParsChanged"
QT_MOC_LITERAL(4, 33, 5), // "about"
QT_MOC_LITERAL(5, 39, 3), // "gpl"
QT_MOC_LITERAL(6, 43, 12), // "TimeStepWrap"
QT_MOC_LITERAL(7, 56, 12), // "togglePaused"
QT_MOC_LITERAL(8, 69, 16), // "setFluxArrowSize"
QT_MOC_LITERAL(9, 86, 4), // "size"
QT_MOC_LITERAL(10, 91, 10), // "RestartSim"
QT_MOC_LITERAL(11, 102, 21), // "toggleShowCellCenters"
QT_MOC_LITERAL(12, 124, 15), // "toggleShowNodes"
QT_MOC_LITERAL(13, 140, 21), // "toggleShowBorderCells"
QT_MOC_LITERAL(14, 162, 16), // "toggleShowFluxes"
QT_MOC_LITERAL(15, 179, 17), // "toggleNodeNumbers"
QT_MOC_LITERAL(16, 197, 17), // "toggleCellNumbers"
QT_MOC_LITERAL(17, 215, 14), // "toggleCellAxes"
QT_MOC_LITERAL(18, 230, 16), // "toggleCellStrain"
QT_MOC_LITERAL(19, 247, 15), // "toggleShowWalls"
QT_MOC_LITERAL(20, 263, 19), // "toggleShowApoplasts"
QT_MOC_LITERAL(21, 283, 14), // "toggleDynCells"
QT_MOC_LITERAL(22, 298, 17), // "toggleMovieFrames"
QT_MOC_LITERAL(23, 316, 18), // "toggleLeafBoundary"
QT_MOC_LITERAL(24, 335, 15), // "toggleHideCells"
QT_MOC_LITERAL(25, 351, 5), // "print"
QT_MOC_LITERAL(26, 357, 15), // "startSimulation"
QT_MOC_LITERAL(27, 373, 14), // "stopSimulation"
QT_MOC_LITERAL(28, 388, 14), // "RefreshInfoBar"
QT_MOC_LITERAL(29, 403, 12), // "readStateXML"
QT_MOC_LITERAL(30, 416, 11), // "const char*"
QT_MOC_LITERAL(31, 428, 8), // "filename"
QT_MOC_LITERAL(32, 437, 8), // "geometry"
QT_MOC_LITERAL(33, 446, 4), // "pars"
QT_MOC_LITERAL(34, 451, 7), // "simtime"
QT_MOC_LITERAL(35, 459, 17), // "EnterRotationMode"
QT_MOC_LITERAL(36, 477, 16), // "ExitRotationMode"
QT_MOC_LITERAL(37, 494, 11), // "UserMessage"
QT_MOC_LITERAL(38, 506, 7), // "message"
QT_MOC_LITERAL(39, 514, 7), // "timeout"
QT_MOC_LITERAL(40, 522, 7), // "Refresh"
QT_MOC_LITERAL(41, 530, 14), // "PauseIfRunning"
QT_MOC_LITERAL(42, 545, 13), // "ContIfRunning"
QT_MOC_LITERAL(43, 559, 15), // "XMLReadSettings"
QT_MOC_LITERAL(44, 575, 8), // "xmlNode*"
QT_MOC_LITERAL(45, 584, 8), // "settings"
QT_MOC_LITERAL(46, 593, 7), // "aboutQt"
QT_MOC_LITERAL(47, 601, 14), // "EditParameters"
QT_MOC_LITERAL(48, 616, 10), // "GetLeafDir"
QT_MOC_LITERAL(49, 627, 4), // "QDir"
QT_MOC_LITERAL(50, 632, 16), // "readNextStateXML"
QT_MOC_LITERAL(51, 649, 16), // "readPrevStateXML"
QT_MOC_LITERAL(52, 666, 17), // "readFirstStateXML"
QT_MOC_LITERAL(53, 684, 16), // "readLastStateXML"
QT_MOC_LITERAL(54, 701, 14), // "exportCellData"
QT_MOC_LITERAL(55, 716, 12), // "saveStateXML"
QT_MOC_LITERAL(56, 729, 8), // "snapshot"
QT_MOC_LITERAL(57, 738, 8), // "savePars"
QT_MOC_LITERAL(58, 747, 8), // "readPars"
QT_MOC_LITERAL(59, 756, 5), // "clear"
QT_MOC_LITERAL(60, 762, 4), // "init"
QT_MOC_LITERAL(61, 767, 6), // "CutSAM"
QT_MOC_LITERAL(62, 774, 7), // "enlarge"
QT_MOC_LITERAL(63, 782, 6), // "shrink"
QT_MOC_LITERAL(64, 789, 6), // "zoomIn"
QT_MOC_LITERAL(65, 796, 7), // "zoomOut"
QT_MOC_LITERAL(66, 804, 9), // "CleanMesh"
QT_MOC_LITERAL(67, 814, 18), // "CleanMeshChemicals"
QT_MOC_LITERAL(68, 833, 21), // "CleanMeshTransporters"
QT_MOC_LITERAL(69, 855, 13) // "RandomizeMesh"

    },
    "Main\0SimulationDone\0\0ParsChanged\0about\0"
    "gpl\0TimeStepWrap\0togglePaused\0"
    "setFluxArrowSize\0size\0RestartSim\0"
    "toggleShowCellCenters\0toggleShowNodes\0"
    "toggleShowBorderCells\0toggleShowFluxes\0"
    "toggleNodeNumbers\0toggleCellNumbers\0"
    "toggleCellAxes\0toggleCellStrain\0"
    "toggleShowWalls\0toggleShowApoplasts\0"
    "toggleDynCells\0toggleMovieFrames\0"
    "toggleLeafBoundary\0toggleHideCells\0"
    "print\0startSimulation\0stopSimulation\0"
    "RefreshInfoBar\0readStateXML\0const char*\0"
    "filename\0geometry\0pars\0simtime\0"
    "EnterRotationMode\0ExitRotationMode\0"
    "UserMessage\0message\0timeout\0Refresh\0"
    "PauseIfRunning\0ContIfRunning\0"
    "XMLReadSettings\0xmlNode*\0settings\0"
    "aboutQt\0EditParameters\0GetLeafDir\0"
    "QDir\0readNextStateXML\0readPrevStateXML\0"
    "readFirstStateXML\0readLastStateXML\0"
    "exportCellData\0saveStateXML\0snapshot\0"
    "savePars\0readPars\0clear\0init\0CutSAM\0"
    "enlarge\0shrink\0zoomIn\0zoomOut\0CleanMesh\0"
    "CleanMeshChemicals\0CleanMeshTransporters\0"
    "RandomizeMesh"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Main[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      63,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  329,    2, 0x06 /* Public */,
       3,    0,  330,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,  331,    2, 0x0a /* Public */,
       5,    0,  332,    2, 0x0a /* Public */,
       6,    0,  333,    2, 0x0a /* Public */,
       7,    0,  334,    2, 0x0a /* Public */,
       8,    1,  335,    2, 0x0a /* Public */,
      10,    0,  338,    2, 0x0a /* Public */,
      11,    0,  339,    2, 0x0a /* Public */,
      12,    0,  340,    2, 0x0a /* Public */,
      13,    0,  341,    2, 0x0a /* Public */,
      14,    0,  342,    2, 0x0a /* Public */,
      15,    0,  343,    2, 0x0a /* Public */,
      16,    0,  344,    2, 0x0a /* Public */,
      17,    0,  345,    2, 0x0a /* Public */,
      18,    0,  346,    2, 0x0a /* Public */,
      19,    0,  347,    2, 0x0a /* Public */,
      20,    0,  348,    2, 0x0a /* Public */,
      21,    0,  349,    2, 0x0a /* Public */,
      22,    0,  350,    2, 0x0a /* Public */,
      23,    0,  351,    2, 0x0a /* Public */,
      24,    0,  352,    2, 0x0a /* Public */,
      25,    0,  353,    2, 0x0a /* Public */,
      26,    0,  354,    2, 0x0a /* Public */,
      27,    0,  355,    2, 0x0a /* Public */,
      28,    0,  356,    2, 0x0a /* Public */,
      29,    4,  357,    2, 0x0a /* Public */,
      29,    3,  366,    2, 0x2a /* Public | MethodCloned */,
      29,    2,  373,    2, 0x2a /* Public | MethodCloned */,
      29,    1,  378,    2, 0x2a /* Public | MethodCloned */,
      35,    0,  381,    2, 0x0a /* Public */,
      36,    0,  382,    2, 0x0a /* Public */,
      37,    2,  383,    2, 0x0a /* Public */,
      37,    1,  388,    2, 0x2a /* Public | MethodCloned */,
      40,    0,  391,    2, 0x0a /* Public */,
      41,    0,  392,    2, 0x0a /* Public */,
      42,    0,  393,    2, 0x0a /* Public */,
      43,    1,  394,    2, 0x0a /* Public */,
      46,    0,  397,    2, 0x08 /* Private */,
      47,    0,  398,    2, 0x08 /* Private */,
      48,    0,  399,    2, 0x08 /* Private */,
      29,    0,  400,    2, 0x08 /* Private */,
      50,    0,  401,    2, 0x08 /* Private */,
      51,    0,  402,    2, 0x08 /* Private */,
      52,    0,  403,    2, 0x08 /* Private */,
      53,    0,  404,    2, 0x08 /* Private */,
      54,    0,  405,    2, 0x08 /* Private */,
      54,    1,  406,    2, 0x08 /* Private */,
      55,    0,  409,    2, 0x08 /* Private */,
      56,    0,  410,    2, 0x08 /* Private */,
      57,    0,  411,    2, 0x08 /* Private */,
      58,    0,  412,    2, 0x08 /* Private */,
      59,    0,  413,    2, 0x08 /* Private */,
      60,    0,  414,    2, 0x08 /* Private */,
      61,    0,  415,    2, 0x08 /* Private */,
      62,    0,  416,    2, 0x08 /* Private */,
      63,    0,  417,    2, 0x08 /* Private */,
      64,    0,  418,    2, 0x08 /* Private */,
      65,    0,  419,    2, 0x08 /* Private */,
      66,    0,  420,    2, 0x08 /* Private */,
      67,    0,  421,    2, 0x08 /* Private */,
      68,    0,  422,    2, 0x08 /* Private */,
      69,    0,  423,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Int, 0x80000000 | 30, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,   31,   32,   33,   34,
    QMetaType::Int, 0x80000000 | 30, QMetaType::Bool, QMetaType::Bool,   31,   32,   33,
    QMetaType::Int, 0x80000000 | 30, QMetaType::Bool,   31,   32,
    QMetaType::Int, 0x80000000 | 30,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   38,   39,
    QMetaType::Void, QMetaType::QString,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 44,   45,
    QMetaType::Void,
    QMetaType::Void,
    0x80000000 | 49,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Main::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Main *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SimulationDone(); break;
        case 1: _t->ParsChanged(); break;
        case 2: _t->about(); break;
        case 3: _t->gpl(); break;
        case 4: _t->TimeStepWrap(); break;
        case 5: _t->togglePaused(); break;
        case 6: _t->setFluxArrowSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->RestartSim(); break;
        case 8: _t->toggleShowCellCenters(); break;
        case 9: _t->toggleShowNodes(); break;
        case 10: _t->toggleShowBorderCells(); break;
        case 11: _t->toggleShowFluxes(); break;
        case 12: _t->toggleNodeNumbers(); break;
        case 13: _t->toggleCellNumbers(); break;
        case 14: _t->toggleCellAxes(); break;
        case 15: _t->toggleCellStrain(); break;
        case 16: _t->toggleShowWalls(); break;
        case 17: _t->toggleShowApoplasts(); break;
        case 18: _t->toggleDynCells(); break;
        case 19: _t->toggleMovieFrames(); break;
        case 20: _t->toggleLeafBoundary(); break;
        case 21: _t->toggleHideCells(); break;
        case 22: _t->print(); break;
        case 23: _t->startSimulation(); break;
        case 24: _t->stopSimulation(); break;
        case 25: _t->RefreshInfoBar(); break;
        case 26: { int _r = _t->readStateXML((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 27: { int _r = _t->readStateXML((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 28: { int _r = _t->readStateXML((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 29: { int _r = _t->readStateXML((*reinterpret_cast< const char*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 30: _t->EnterRotationMode(); break;
        case 31: _t->ExitRotationMode(); break;
        case 32: _t->UserMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 33: _t->UserMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 34: _t->Refresh(); break;
        case 35: _t->PauseIfRunning(); break;
        case 36: _t->ContIfRunning(); break;
        case 37: _t->XMLReadSettings((*reinterpret_cast< xmlNode*(*)>(_a[1]))); break;
        case 38: _t->aboutQt(); break;
        case 39: _t->EditParameters(); break;
        case 40: { QDir _r = _t->GetLeafDir();
            if (_a[0]) *reinterpret_cast< QDir*>(_a[0]) = std::move(_r); }  break;
        case 41: _t->readStateXML(); break;
        case 42: _t->readNextStateXML(); break;
        case 43: _t->readPrevStateXML(); break;
        case 44: _t->readFirstStateXML(); break;
        case 45: _t->readLastStateXML(); break;
        case 46: _t->exportCellData(); break;
        case 47: _t->exportCellData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 48: _t->saveStateXML(); break;
        case 49: _t->snapshot(); break;
        case 50: _t->savePars(); break;
        case 51: _t->readPars(); break;
        case 52: _t->clear(); break;
        case 53: _t->init(); break;
        case 54: _t->CutSAM(); break;
        case 55: _t->enlarge(); break;
        case 56: _t->shrink(); break;
        case 57: _t->zoomIn(); break;
        case 58: _t->zoomOut(); break;
        case 59: _t->CleanMesh(); break;
        case 60: _t->CleanMeshChemicals(); break;
        case 61: _t->CleanMeshTransporters(); break;
        case 62: _t->RandomizeMesh(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Main::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Main::SimulationDone)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Main::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Main::ParsChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Main::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_Main.data,
    qt_meta_data_Main,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Main::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Main::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Main.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "MainBase"))
        return static_cast< MainBase*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Main::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 63)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 63;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 63)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 63;
    }
    return _id;
}

// SIGNAL 0
void Main::SimulationDone()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Main::ParsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
