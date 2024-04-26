#pragma once




#include <QTableView>

class FreezeSortTableView : public QTableView {
Q_OBJECT
public:
    FreezeSortTableView(QAbstractItemModel *model, QAbstractItemDelegate* delegate = nullptr, QWidget *parent = nullptr);

    ~FreezeSortTableView();

    void setSortAble(bool enable);

    void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior);
    void setSelectionMode(QAbstractItemView::SelectionMode mode);

protected:
    void resizeEvent(QResizeEvent *event) override;

    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;

    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

private:
    void init();

    void updateFrozenTableGeometry();

private slots:

    void updateSectionWidth(int logicalIndex, int oldSize, int newSize);

    void updateSectionHeight(int logicalIndex, int oldSize, int newSize);

private:
    QTableView *m_pFrozenTableView = nullptr;
};
