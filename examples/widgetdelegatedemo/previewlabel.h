#ifndef PREVIEWLABEL_H
#define PREVIEWLABEL_H

#include <memory>
#include <QAbstractButton>

//
// This class is a simplified example of
// creating QtItemDelegate compatiable
// widget with custom painting.
//
class PreviewLabel : public QAbstractButton
{
    Q_OBJECT
public:
    // Shape of mask of label
    enum class MaskShape
    {
        NoShape, // rectangle shape - no mask at all (default)
        Circle,  // circle mask
        Rounded  // rectangle with rounded corners
    };

    explicit PreviewLabel(QWidget* parent = Q_NULLPTR);
    ~PreviewLabel();

    void setMaskShape(MaskShape shape);
    MaskShape maskShape() const;

    void setPixmap(const QPixmap& pixmap, Qt::TransformationMode mode = Qt::FastTransformation);
    QPixmap pixmap() const;

    void setLoading(bool on);
    bool isLoading() const;

public Q_SLOTS:
    void updateProgress();

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    std::unique_ptr<class PreviewLabelPrivate> d;
};

#endif // PREVIEWLABEL_H
