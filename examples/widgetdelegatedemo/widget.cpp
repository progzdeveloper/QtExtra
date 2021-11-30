#include "widget.h"
#include "qtfilelistmodel.h"
#include "previewlabel.h"

#include <QMouseEvent>

//
// Example of usage of QtWidgetItemDelegate
//
// In this example we create a file list model and
// setup our custom FileItemDelegate on QTableView.
// The delegate itself is using our FileItemWidget to
// render contents of a single item. Also this widget
// has a number of clickable buttons, and a custom
// preview widget.
// That demonstrates possibilities of using custom painted
// widgets and usage of signal/slot connections inside item.
// The interesting part is that custom style-sheets, hover
// and custom cursors should work (but still a little buggy).
//
// Points of interest:
// - We also can trigger context menu from within clicking
// the button inside item. The menu can be displayed around
// item rect or button rect (this is simply a matter of geometry
// calculations), also the QToolButton with menu will work, but
// be aware of that only QToolButton::InstantPopup are currently
// supported. See moreButton in FileItemWidget and menu related
// signals/slots in FileWidgetItem and FileItemDelegate classes.
//
// - Another point of interest is custom event handling inside FileWidgetItem
// see FileWidgetItem::viewportEvent() for details.
//
// - Yet another point of interest is animation on items.
// This can be achieved by using a common timer inside a delegate,
// see FileItemDelegate::updateProgress(), FileItemDelegate::onDataChanged()
// and FileItemDelegate::renderHint() for details.
//
Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    model = new QtFileListModel(this);

    // Example of view with QtWidgetItemDelegate
    // that suppose to display dynamic content with
    // clickable elements (e.g. buttons)
    tableView = new QTableView(this);
    tableView->installEventFilter(this);
    tableView->viewport()->installEventFilter(this);
    tableView->viewport()->setMouseTracking(true);

    delegate = new FileItemDelegate(tableView);
    delegate->setAutoFillBackground(false);
    delegate->setStaticContent(false);

    connect(delegate, &FileItemDelegate::removeIndex, this, &Widget::removeItem);
    connect(delegate, &FileItemDelegate::showItemMenu, this, &Widget::showContextMenu);
    connect(delegate, &FileItemDelegate::showItemMenuAt, this, &Widget::showContextMenuAt);
    connect(delegate, &FileItemDelegate::showItemMenuAround, this, &Widget::showContextMenuAround);

    tableView->setItemDelegate(delegate);
    connect(model, &QtFileListModel::modelReset, this, &Widget::onModelReset);
    connect(model, &QtFileListModel::dataChanged, delegate, &FileItemDelegate::invalidateRange);
    connect(model, &QtFileListModel::dataChanged, delegate, &FileItemDelegate::onDataChanged);

    tableView->setGridStyle(Qt::NoPen);
    tableView->horizontalHeader()->setHidden(true);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableView->verticalHeader()->setHidden(true);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    tableView->setModel(model);

    // Example of view with QtWidgetItemDelegate
    // that suppose to display static content
    // *without* any clickable elements (e.g. buttons)
    listView = new QListView(this);
    // mouse tracking required to mouse hover events work properly
    listView->setMouseTracking(true);
    listView->setModel(model);
    listView->setItemDelegate(new ItemWidgetDelegate(listView));


    QPushButton* openButton = new QPushButton(tr("Select Images/Files..."), this);
    connect(openButton, &QPushButton::clicked, this, &Widget::selectFiles);

    QPushButton* clearButton = new QPushButton(tr("Clear"), this);
    connect(clearButton, &QPushButton::clicked, model, &QtFileListModel::reset);

    label = new QLabel(this);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->addWidget(openButton);
    buttonsLayout->addWidget(clearButton);
    buttonsLayout->addWidget(label);
    buttonsLayout->addStretch();

    QTabWidget* container = new QTabWidget(this);
    container->addTab(tableView, tr("Dynamic Contents Delegate"));
    container->addTab(listView, tr("Static Contents Delegate"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(buttonsLayout);
    layout->addWidget(container);

    setMinimumWidth(456);
}

Widget::~Widget()
{
}

void Widget::selectFiles()
{
    const QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Files"));
    if (files.isEmpty())
        return;

    model->setFiles(files);
}

void Widget::onModelReset()
{
    delegate->invalidate();
    if (model->rowCount() != 0) // trigger the animation
        delegate->onDataChanged(model->index(0), model->index(model->rowCount()-1));
    label->setText(tr("Total items in model: %1").arg(model->rowCount()));
}

void Widget::removeItem(const QModelIndex &index)
{
    if (index.isValid())
        model->removeRow(index.row());

    label->setText(tr("Total items in model: %1").arg(model->rowCount()));
}

void Widget::showContextMenu(const QModelIndex &index)
{
    QRect rectHint = tableView->visualRect(index);
    rectHint.moveTo(tableView->viewport()->mapToGlobal(rectHint.topLeft()));
    showContextMenuAround(index, rectHint);
}

void Widget::showContextMenuAround(const QModelIndex &index, const QRect& rectHint)
{
    QMenu* menu = createMenu(index);

    // here we determine where menu must be shown
    // in this case we use bottom-right point and
    // show menu in down direction, but we
    // can use any of rect point and direction,
    // just a matter of geometry computations :)
    const QPoint pos = rectHint.bottomRight() - QPoint(menu->width(), 0);

    // move to desired position
    menu->move(pos);
    // show and adjust menu size
    menu->show();
    menu->adjustSize();
}

void Widget::showContextMenuAt(const QModelIndex& index, const QPoint& globalPos)
{
    QMenu* menu = createMenu(index);
    // move to desired position
    menu->move(globalPos);
    // show and adjust menu size
    menu->show();
    menu->adjustSize();
}

QMenu *Widget::createMenu(const QModelIndex &index) const
{
    Widget* that = const_cast<Widget*>(this);
    QMenu* menu = new QMenu(const_cast<Widget*>(this));
    connect(menu, &QMenu::destroyed, tableView, &QWidget::unsetCursor);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    // here we need to create
    // (may be specific for index)
    // actions for menu
    menu->addAction("Action1");
    menu->addAction("Action2");
    menu->addSeparator();
    menu->addAction("Remove", this, [that, index] () { that->removeItem(index); });
    menu->adjustSize();

    return menu;
}

bool Widget::eventFilter(QObject *object, QEvent *event)
{
    // this is an example of handling menu key press for
    // showing custom context menu
    if (object == tableView && event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Menu)
        {
            // find the position and index
            const QPoint globalPos = QCursor::pos();
            const QPoint localPos = tableView->viewport()->mapFromGlobal(globalPos);
            const QAbstractItemModel* viewModel = tableView->model();
            const QModelIndex buddy = viewModel->buddy(tableView->indexAt(localPos));

            showContextMenuAt(buddy, globalPos);
            return true; // filter event out
        }
    }

    return QWidget::eventFilter(object, event);
}


//
// FileItemWidget implementation
//
FileItemWidget::FileItemWidget(bool isStatic, QWidget *parent) : QtItemWidget(parent), staticContents(isStatic)
{
    QFont sizeFont = font();
    sizeFont.setItalic(true);
    sizeFont.setPixelSize(10);

    QFont pathFont = sizeFont;
    pathFont.setPixelSize(pathFont.pixelSize() + 4);

    previewLabel = new PreviewLabel(this);
    previewLabel->setMaskShape(PreviewLabel::MaskShape::Circle);
    previewLabel->setFixedSize(64, 64);

    pathLabel = new QLabel(this);
    pathLabel->setFixedWidth(176);
    pathLabel->setFont(pathFont);

    sizeLabel = new QLabel(this);
    sizeLabel->setFont(sizeFont);

    sampleButton = new QPushButton(tr("Push Me!"), this);
    sampleButton->setObjectName("sampleButton");

    // we can use custom cursors
    sampleButton->setCursor(Qt::PointingHandCursor);

    // we can use custom style sheets
    sampleButton->setStyleSheet("QPushButton{background:green}QPushButton:hover{background:yellow}QPushButton:pressed{background:red}");
    connect(sampleButton, &QPushButton::clicked, this, []() { qDebug() << "sampleButton clicked"; });

    moreButton = new QToolButton(this);
    moreButton->setObjectName("moreButton");
    moreButton->setText("...");
    moreButton->setCursor(Qt::PointingHandCursor);
    moreButton->setAutoRaise(true);

    // Here we need to decide where we want menu to appear (uncomment one of possibilities):
    // - this one show context menu around item rect
    //connect(moreButton, &QToolButton::clicked, this, &FileItemWidget::requestContextMenu);
    // - this one show context menu around button rect
    connect(moreButton, &QToolButton::clicked, this, [this]()
    {
        QRect r = moreButton->rect();
        r.moveTo(moreButton->mapToGlobal({0, 0}));
        Q_EMIT requestContextMenuAround(r);
    });

    removeButton = new QToolButton(this);
    removeButton->setObjectName("removeButton");
    removeButton->setCursor(Qt::PointingHandCursor);
    removeButton->setAutoRaise(true);
    removeButton->setIcon(QIcon::fromTheme("window-close"));
    connect(removeButton, &QToolButton::clicked, this, &FileItemWidget::removeItem);

    QVBoxLayout* labelsLayout = new QVBoxLayout;
    labelsLayout->addStretch();
    labelsLayout->addWidget(pathLabel);
    labelsLayout->addWidget(sizeLabel);
    labelsLayout->addStretch();

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(previewLabel);
    mainLayout->addLayout(labelsLayout, 1);
    mainLayout->addWidget(sampleButton);
    mainLayout->addWidget(moreButton);
    mainLayout->addWidget(removeButton);

    selectedPalette = palette();
    selectedPalette.setColor(QPalette::Window, selectedPalette.color(QPalette::Highlight));
    selectedPalette.setColor(QPalette::WindowText, selectedPalette.color(QPalette::HighlightedText));

    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground);
}

void FileItemWidget::setData(const QModelIndex &index, const QStyleOptionViewItem& option)
{
    static QSize smallIconSize(64, 64);
    static QSize largeIconSize(256, 256);

    const QAbstractItemModel* model = index.model();

    const bool isSingle = (model->rowCount() == 1); // only single item in model

    if (const auto area = qobject_cast<QAbstractScrollArea*>(const_cast<QWidget*>(option.widget)))
        area->setVerticalScrollBarPolicy(isSingle ? Qt::ScrollBarAlwaysOff : Qt::ScrollBarAsNeeded);

    const bool isLoading = index.data(QtFileListModel::FileStatusRole).toBool();
    previewLabel->setLoading(isLoading);
    sampleButton->setEnabled(!isLoading);
    moreButton->setEnabled(!isLoading);

    // we can use palettes to simulate delegate selected state
    if (option.state & QStyle::State_Selected)
    {
        pathLabel->setPalette(selectedPalette);
        sizeLabel->setPalette(selectedPalette);
    }
    else
    {
        pathLabel->setPalette(palette());
        sizeLabel->setPalette(palette());
    }

    QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
    if (staticContents)
    {
        // simple data setup in case of static contents:
        // here we simple setup model data in non-clickable widgets
        previewLabel->setFixedSize(smallIconSize);
        previewLabel->setPixmap(pixmap, Qt::SmoothTransformation);
        pathLabel->setText(elidedText(index.data(Qt::DisplayRole).toString()));
        sizeLabel->setText(index.data(QtFileListModel::FileSizeRole).toString());

        pathLabel->setVisible(true);
        sizeLabel->setVisible(true);

        // hide any clickable elements
        sampleButton->setVisible(false);
        moreButton->setVisible(false);
        removeButton->setVisible(false);
    }
    else
    {
        // show that we can change appearence along with widget size in case of dynamic content
        if (isSingle && index.data(QtFileListModel::FileTypeRole).toBool())
        {
            // example of custom behavior: if number of rows is 1,
            // and file type is media then show large icon and hide some buttons

            previewLabel->setFixedSize(largeIconSize);
            previewLabel->setPixmap(pixmap, Qt::FastTransformation);

            pathLabel->setVisible(false);
            sizeLabel->setVisible(false);

            removeButton->setVisible(false);
        }
        else
        {
            previewLabel->setFixedSize(smallIconSize);
            previewLabel->setPixmap(pixmap, Qt::SmoothTransformation);
            pathLabel->setText(elidedText(index.data(Qt::DisplayRole).toString()));
            sizeLabel->setText(index.data(QtFileListModel::FileSizeRole).toString());

            pathLabel->setVisible(true);
            sizeLabel->setVisible(true);
            // show remove button only when mouse hover over item
            removeButton->setVisible(option.state & QStyle::State_MouseOver);
        }
    }
}

void FileItemWidget::updateProgress()
{
    previewLabel->updateProgress();
}

bool FileItemWidget::viewportEvent(QEvent *e, QWidget *w)
{
    // example of custom item event processing
    if (e->type() == QEvent::MouseButtonPress)
    {
        // show context menu on mouse right button click
        QMouseEvent* pressEvent = static_cast<QMouseEvent*>(e);
        if (pressEvent->button() == Qt::RightButton)
        {
            Q_EMIT requestContextMenuAt(pressEvent->globalPos());
            return true;
        }
    }
    return QtItemWidget::viewportEvent(e, w);
}

QString FileItemWidget::elidedText(const QString &text) const
{
    QFontMetrics metrics(pathLabel->font());
    const int width = pathLabel->width() - 2;
    return metrics.elidedText(text, Qt::ElideRight, width);
}

//
// class ItemWidgetDelegate implementation
// This is show how it's easy to implement a custom
// delegate: only a few methods overloading is required!
//
ItemWidgetDelegate::ItemWidgetDelegate(QObject *parent) : QtWidgetItemDelegate(parent)
{
    setStaticContent(true);
}

QtItemWidget *ItemWidgetDelegate::createItemWidget() const
{
    return new FileItemWidget(true);
}


//
// FileItemDelegate implementation
//
// This is a more complicated since we want
// more control over appearance and behavior
//
FileItemDelegate::FileItemDelegate(QObject *parent) : QtWidgetItemDelegate(parent), itemWidget(Q_NULLPTR)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &FileItemDelegate::updateProgress);
}

void FileItemDelegate::itemRemoveRequest()
{
    if (auto view = qobject_cast<QAbstractItemView*>(parent()))
    {
        const QPoint globalPoint = QCursor::pos();
        const QPoint localPoint = view->mapFromGlobal(globalPoint);
        Q_EMIT removeIndex(view->indexAt(localPoint));

        view->doItemsLayout();
    }
}

void FileItemDelegate::itemMenuRequest()
{
    if (auto view = qobject_cast<QAbstractItemView*>(parent()))
    {
        const QPoint globalPoint = QCursor::pos();
        const QPoint localPoint = view->mapFromGlobal(globalPoint);
        Q_EMIT showItemMenu(view->indexAt(localPoint));
    }
}

void FileItemDelegate::itemMenuRequestAt(const QPoint &globalPos)
{
    if (auto view = qobject_cast<QAbstractItemView*>(parent()))
    {
        const QPoint localPoint = view->mapFromGlobal(globalPos);
        Q_EMIT showItemMenuAt(view->indexAt(localPoint), globalPos);
    }
}

void FileItemDelegate::itemMenuRequestAround(const QRect& rectHint)
{
    if (auto view = qobject_cast<QAbstractItemView*>(parent()))
    {
        const QPoint globalPoint = QCursor::pos();
        const QPoint localPoint = view->mapFromGlobal(globalPoint);
        Q_EMIT showItemMenuAround(view->indexAt(localPoint), rectHint);
    }
}

void FileItemDelegate::updateProgress()
{
    if (!itemWidget)
        return;

    // update the progress - this allow to see animation spinning
    itemWidget->updateProgress();

    if (auto view = qobject_cast<QAbstractItemView*>(parent()))
    {
        auto viewport = view->viewport();
        if (!viewport)
            return;

        const auto model = view->model();
        if (!model)
            return;

        // repaint all indices that need to be repainted
        for (auto it = lookup.cbegin(); it != lookup.cend(); ++it)
        {
            const quint64 hashCode = *it;
            const int row = static_cast<int>(hashCode);
            const int column = static_cast<int>(hashCode >> 32);

            QModelIndex index = model->index(row, column);
            if (index.isValid())
                viewport->repaint(view->visualRect(index));
        }
    }
}

QSize FileItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    createWidgetOnDemand();
    // example of overriding sizeHint() method
    if (index.isValid() && index.model()->rowCount() == 1)
    {
        updateWidgetData(index, option); // update widget data
        widget()->adjustSize(); // adjust size to get correct widget minimum size
        return widget()->minimumSize();
    }
    return QtWidgetItemDelegate::sizeHint(option, index);
}

QtWidgetItemDelegate::RenderHint FileItemDelegate::renderHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // example of overriding renderHint() method
    if (index.isValid() && index.data(QtFileListModel::FileStatusRole).toBool())
        return RenderHint::RenderDirect; // draw direct if item is loading - no need to cache animation
    else
        return QtWidgetItemDelegate::renderHint(option, index);
}

QtItemWidget *FileItemDelegate::createItemWidget() const
{
    itemWidget = new FileItemWidget(false);
    connect(itemWidget, &FileItemWidget::removeItem, this, &FileItemDelegate::itemRemoveRequest);
    connect(itemWidget, &FileItemWidget::requestContextMenu, this, &FileItemDelegate::itemMenuRequest);
    connect(itemWidget, &FileItemWidget::requestContextMenuAt, this, &FileItemDelegate::itemMenuRequestAt);
    connect(itemWidget, &FileItemWidget::requestContextMenuAround, this, &FileItemDelegate::itemMenuRequestAround);
    return itemWidget;
}

void FileItemDelegate::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    // example of using animation with QtWidgetItemDelegate

    QModelIndex index = topLeft;
    for (int i = index.row(), n = bottomRight.row(); i <= n; i++)
    {
        const quint64 hashCode = quint64(i);
        // check if item is being loading
        const bool loading = index.sibling(i, 0).data(QtFileListModel::FileStatusRole).toBool();
        if (loading) // update lookup
            lookup.emplace(hashCode);
        else
            lookup.erase(hashCode);
    }

    if (lookup.empty())
    {
        // if lookup is empty, than all model
        // elements are loaded - stop the timer
        // if isn't stopped already
        if (timer->isActive())
            timer->stop();
    }
    else
    {
        // if lookup is not empty, than some
        // elements are still loading so start
        // the timer, if isn't started already
        if (!timer->isActive())
            timer->start(100);
    }
}

