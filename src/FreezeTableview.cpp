#include "FreezeTableview.h"


#include <QDebug>
#include <QScrollBar>
#include <QHeaderView>
#include <QStyleFactory>
#include <QItemSelectionModel>

FreezeSortTableView::FreezeSortTableView(QAbstractItemModel *model, QAbstractItemDelegate *delegate, QWidget *parent)
        : QTableView(parent) {
    setModel(model);
    if (delegate) setItemDelegate(delegate);
    m_pFrozenTableView = new QTableView(this);
    m_pFrozenTableView->setStyleSheet("QTableView { "
                                          "border-left:none;"
                                          "border-top:none;"
                                          "border-bottom:none;"
                                          "border-right: 1px solid #B9B9B9;"
                                          "background-color: #E5E5E5;"
                                          "selection-background-color: palette(highlight);"
                                          "selection-color: palette(highlightedText);"
                                      "}");
    connect(horizontalHeader(), &QHeaderView::sectionResized, this,
            &FreezeSortTableView::updateSectionWidth);
    connect(verticalHeader(), &QHeaderView::sectionResized, this,
            &FreezeSortTableView::updateSectionHeight);
    connect(m_pFrozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged,
            verticalScrollBar(), &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
            m_pFrozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);
    init();
}

FreezeSortTableView::~FreezeSortTableView() {
    delete m_pFrozenTableView;
}

void FreezeSortTableView::init() {
    m_pFrozenTableView->setModel(model());
    m_pFrozenTableView->setItemDelegate(itemDelegate());
    m_pFrozenTableView->setFocusPolicy(Qt::NoFocus);
    verticalHeader()->hide();
    m_pFrozenTableView->verticalHeader()->hide();
    m_pFrozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    viewport()->stackUnder(m_pFrozenTableView);

    m_pFrozenTableView->setSelectionModel(selectionModel());
    for (int col = 1; col < model()->columnCount(); ++col)
        m_pFrozenTableView->setColumnHidden(col, true);
    setColumnWidth(0, 70);
    m_pFrozenTableView->setColumnWidth(0, columnWidth(0));
    m_pFrozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pFrozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pFrozenTableView->show();
    updateFrozenTableGeometry();
    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    m_pFrozenTableView->setVerticalScrollMode(ScrollPerPixel);
}

void FreezeSortTableView::setSelectionMode(QAbstractItemView::SelectionMode mode) {
    QAbstractItemView::setSelectionMode(mode);
    m_pFrozenTableView->setSelectionMode(mode);
}

void FreezeSortTableView::setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior) {
    QAbstractItemView::setSelectionBehavior(behavior);
    m_pFrozenTableView->setSelectionBehavior(behavior);

    if (behavior != QAbstractItemView::SelectItems) {
        connect(selectionModel(), &QItemSelectionModel::selectionChanged,
                [this](const QItemSelection &selected, const QItemSelection &deselected) {
                    m_pFrozenTableView->selectionModel()->select(selected, QItemSelectionModel::Select);
                    m_pFrozenTableView->selectionModel()->select(deselected, QItemSelectionModel::Deselect);
                });
        connect(m_pFrozenTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
                [this](const QItemSelection &selected, const QItemSelection &deselected) {
                    selectionModel()->select(selected, QItemSelectionModel::Select);
                    selectionModel()->select(deselected, QItemSelectionModel::Deselect);
                });
    } else {
        disconnect(selectionModel(), &QItemSelectionModel::selectionChanged, nullptr, nullptr);
        disconnect(m_pFrozenTableView->selectionModel(), &QItemSelectionModel::selectionChanged, nullptr, nullptr);
    }
}

void FreezeSortTableView::setSortAble(bool enable) {
    setSortingEnabled(enable);
    m_pFrozenTableView->setSortingEnabled(enable);
    // sync sort indicator
    if (enable) {
        connect(horizontalHeader(), &QHeaderView::sortIndicatorChanged, m_pFrozenTableView->horizontalHeader(),
                &QHeaderView::setSortIndicator, Qt::UniqueConnection);
        connect(m_pFrozenTableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, horizontalHeader(),
                &QHeaderView::setSortIndicator, Qt::UniqueConnection);
        connect(horizontalHeader(), &QHeaderView::sortIndicatorChanged, this,
                &FreezeSortTableView::updateFrozenTableGeometry, Qt::UniqueConnection);
        connect(m_pFrozenTableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this,
                &FreezeSortTableView::updateFrozenTableGeometry, Qt::UniqueConnection);
        horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
    } else {
        disconnect(horizontalHeader(), &QHeaderView::sortIndicatorChanged, m_pFrozenTableView->horizontalHeader(),
                   &QHeaderView::setSortIndicator);
        disconnect(m_pFrozenTableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, horizontalHeader(),
                   &QHeaderView::setSortIndicator);
        disconnect(horizontalHeader(), &QHeaderView::sortIndicatorChanged, this,
                   &FreezeSortTableView::updateFrozenTableGeometry);
        disconnect(m_pFrozenTableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this,
                   &FreezeSortTableView::updateFrozenTableGeometry);
    }
}


void FreezeSortTableView::updateSectionWidth(int logicalIndex, int /* oldSize */, int newSize) {
    if (logicalIndex == 0) {
        m_pFrozenTableView->setColumnWidth(0, newSize);
        updateFrozenTableGeometry();
    }
}

void FreezeSortTableView::updateSectionHeight(int logicalIndex, int /* oldSize */, int newSize) {
    m_pFrozenTableView->setRowHeight(logicalIndex, newSize);
}

void FreezeSortTableView::resizeEvent(QResizeEvent *event) {
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}

QModelIndex FreezeSortTableView::moveCursor(CursorAction cursorAction,
                                            Qt::KeyboardModifiers modifiers) {
    QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

    if (cursorAction == MoveLeft && current.column() > 0
        && visualRect(current).topLeft().x() < m_pFrozenTableView->columnWidth(0)) {
        const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
                             - m_pFrozenTableView->columnWidth(0);
        horizontalScrollBar()->setValue(newValue);
    }
    return current;
}

void FreezeSortTableView::scrollTo(const QModelIndex &index, ScrollHint hint) {
    if (index.column() > 0)
        QTableView::scrollTo(index, hint);
}


void FreezeSortTableView::updateFrozenTableGeometry() {
    m_pFrozenTableView->setGeometry(verticalHeader()->width() + frameWidth(),
                                    frameWidth(), columnWidth(0),
                                    viewport()->height() + horizontalHeader()->height());
}
