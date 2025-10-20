#include "drawingcanvas.h"
#include <QDebug>
#include <vector>
#include <iostream>
#include <iomanip>

DrawingCanvas::DrawingCanvas(QWidget *parent) {
    // Set a minimum size for the canvas
    setMinimumSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    // Set a solid background color
    setStyleSheet("background-color: white; border: 1px solid gray;");
}

void DrawingCanvas::clearPoints(){
    m_points.clear();
    m_segmentCandidates.clear();
    isDetectionClicked = false;
    // Trigger a repaint to clear the canvas
    update();
}

void DrawingCanvas::paintLines(){
    /* Implement lines drawing per even pair */
    isPaintLinesClicked = true;
    isDetectionClicked = false;
    update();
}

// Helper function to define the ideal segment filter patterns (Objective 2)
QVector<CustomMatrix> getSegmentFilters() {
    QVector<CustomMatrix> filters;

    // T = True (Pixel is drawn), F = False (White background)
    // 1. Single Pixel (Center)
    bool p1[3][3] = {{0,0,0}, {0,1,0}, {0,0,0}}; filters.append(CustomMatrix(p1));
    // 2. Horizontal Line Segment (3 pixels wide)
    bool p2[3][3] = {{0,0,0}, {1,1,1}, {0,0,0}}; filters.append(CustomMatrix(p2));
    // 3. Vertical Line Segment (3 pixels tall)
    bool p3[3][3] = {{0,1,0}, {0,1,0}, {0,1,0}}; filters.append(CustomMatrix(p3));
    // 4. Main Diagonal Line Segment
    bool p4[3][3] = {{1,0,0}, {0,1,0}, {0,0,1}}; filters.append(CustomMatrix(p4));
    // 5. Anti-Diagonal Line Segment
    bool p5[3][3] = {{0,0,1}, {0,1,0}, {1,0,0}}; filters.append(CustomMatrix(p5));

    // 6. Corner/Elbow
    bool p6[3][3] = {{1,1,0}, {1,1,0}, {0,0,0}}; filters.append(CustomMatrix(p6));
    // 7. Vertical L-Shape (Top-Left corner)
    bool p7[3][3] = {{1,1,0}, {1,0,0}, {0,0,0}}; filters.append(CustomMatrix(p7));

    // 8. Include 2-pixel edges for thinner segments
    bool p8[3][3] = {{0,0,0}, {1,1,0}, {0,0,0}}; filters.append(CustomMatrix(p8)); // Mid-H 2px
    bool p9[3][3] = {{0,1,0}, {0,1,0}, {0,0,0}}; filters.append(CustomMatrix(p9)); // Mid-V 2px

    return filters;
}


void DrawingCanvas::segmentDetection(){
    // Clear previous candidates and reset flag
    m_segmentCandidates.clear();
    isDetectionClicked = false;

    QPixmap pixmap = this->grab();
    QImage image = pixmap.toImage();

    cout << "image width " << image.width() << endl;
    cout << "image height " << image.height() << endl;

    // Get the predefined segment patterns (Filter Windows)
    QVector<CustomMatrix> filterWindows = getSegmentFilters();

    // Counter for non-empty windows for Objectives 1 & 2
    int non_empty_window_count = 0;

    // Get the pixel value as an ARGB integer (QRgb is a typedef for unsigned int)
    for(int i = 1; i < image.width()-1;i++){
        for(int j = 1; j < image.height()-1;j++){
            bool local_window[3][3] = {false};
            bool is_non_empty = false;

            for(int m=-1;m<=1;m++){
                for(int n=-1;n<=1;n++){
                    // Check bounds to prevent crashing near edges
                    if (i+m >= 0 && i+m < image.width() && j+n >= 0 && j+n < image.height()) {
                        QRgb rgbValue = image.pixel(i+m, j+n);
                        bool is_filled = (rgbValue != 0xffffffff);
                        local_window[m+1][n+1] = is_filled;
                        if (is_filled) {
                            is_non_empty = true; // Flag for Objectives 1 & 2
                        }
                    } else {
                        // Handle boundary case (should be minimal with 1-pixel margin, but good practice)
                        local_window[m+1][n+1] = false;
                    }
                }
            }

            // Objective 1 & 2: Dump all non-empty windows to be assessed.
            if (is_non_empty) {
                non_empty_window_count++;
                CustomMatrix mat(local_window);

                // Print the matrix to console for "dumping" requirement
                // cout << "Window at (" << i << ", " << j << "):" << endl;
                // for (int row = 0; row < 3; ++row) {
                //     for (int col = 0; col < 3; ++col) {
                //         cout << (mat.mat[row][col] ? "1" : "0") << " ";
                //     }
                //     cout << endl;
                // }

                // Objective 3: Match against ideal patterns
                for (const auto& filter : filterWindows) {
                    if (mat == filter) {
                        m_segmentCandidates.append(QPoint(i, j));
                        break; // Stop checking filters, move to next pixel
                    }
                }
            }
        }
    }

    cout << "Total non-empty windows analyzed: " << non_empty_window_count << endl;
    cout << "Detected " << m_segmentCandidates.size() << " segment candidate points." << endl;

    isDetectionClicked = true;
    update(); // Trigger paintEvent to draw the candidates
}

void DrawingCanvas::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set up the pen and brush for drawing the points
    QPen pen(Qt::blue, 5);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::blue));

    // Draw a small circle at each stored point
    for (const QPoint& point : std::as_const(m_points)) {
        painter.drawEllipse(point, 3, 3);
    }

    if(isPaintLinesClicked){
        cout << "paint lines block is called" << endl;
        pen.setColor(Qt::red);
        pen.setWidth(4); // 4-pixel wide line
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);

        // Set the painter's pen to our custom pen.
        painter.setPen(pen);

        for(int i=0;i<m_points.size()-1;i+=2){
            //cout << m_points[i].x() << endl;
            painter.drawLine(m_points[i], m_points[i+1]);
        }
        isPaintLinesClicked = false;
    }

    // Draw detected segment candidates (Objective 3 Visualization)
    if (isDetectionClicked) {
        // Use purple color (Qt::darkMagenta is a good shade of purple)
        QPen purplePen(Qt::darkMagenta, 1);
        QBrush purpleBrush(Qt::darkMagenta);
        painter.setPen(purplePen);
        painter.setBrush(purpleBrush);

        // Draw a small 4x4 rectangle for each candidate point
        for (const QPoint& point : std::as_const(m_segmentCandidates)) {
            painter.drawRect(point.x() - 2, point.y() - 2, 4, 4);
        }
    }

    // return painter pen to blue
    pen.setColor(Qt::blue);
    painter.setPen(pen);
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    // Add the mouse click position to our vector of points
    m_points.append(event->pos());
    // Reset detection state when new points are drawn
    isDetectionClicked = false;
    m_segmentCandidates.clear();
    // Trigger a repaint
    update();
}
