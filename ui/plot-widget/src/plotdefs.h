#ifndef PLOT_DEFS_H
#define PLOT_DEFS_H

#include <qcolor.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

namespace Plot {

/*!
 * \brief Default color palette for curves when no color is specified.
 */
const std::vector<QColor> default_palette = {
    QColor(0x007AFF), // Blue
    QColor(0xFF3B30), // Red
    QColor(0x34C759), // Green
    QColor(0xAF52DE), // Purple
    QColor(0xFF9500), // Orange
    QColor(0x5856D6), // Indigo
    QColor(0xFFCC00), // Yellow
    QColor(0x5AC8FA), // Light Blue
    QColor(0x004080), // Navy
    QColor(0x801A15), // Maroon
    QColor(0x1E7033), // Forest
    QColor(0x663399), // Deep Plum
    QColor(0xB36B00), // Rust
    QColor(0x333399), // Royal Blue
    QColor(0x998000), // Dark Gold
    QColor(0x008B8B), // Dark Cyan
    QColor(0x7fbfff), // Sky
    QColor(0xff9d97), // Salmon
    QColor(0x99e3ab), // Mint
    QColor(0xd7a9ee), // Lavender
    QColor(0xffca80), // Peach
    QColor(0x8080ff), // Periwinkle
    QColor(0xffff80), // Pale Yellow
    QColor(0xa6edff), // Ice Blue
    QColor(0xFF2D55), // Rose
    QColor(0xA2845E), // Tan
    QColor(0x00C7BE), // Teal
    QColor(0xFF00FF), // Magenta
    QColor(0x00FF00), // Lime
    QColor(0x00FFFF), // Cyan
    QColor(0x708090), // Slate
    QColor(0xFFD700)  // Gold
};

/*!
 * \brief Configuration settings for a specific plot curve.
 */
struct CurveConfig {
    bool use_antialiasing = true;               //!< Toggle anti-aliased rendering.
    float line_width = 1.5f;                    //!< Thickness of the curve line.
    double z_order = 0.0;                       //!< Rendering priority (higher is on top).
    double y_offset = 0.0;                      //!< Offset from the actual Y value
    QColor color = Qt::transparent;             //!< Curve color.
    Qt::PenStyle line_style = Qt::SolidLine;    //!< Style of the line (Solid, Dashed, etc).
    QwtPlotCurve::CurveStyle curve_style = QwtPlotCurve::Lines; //!< Qwt curve drawing style.
    QwtSymbol::Style symbol_type = QwtSymbol::NoSymbol;         //!< Type of point symbols.
};

/*!
 * \brief Container for curve data and its associated configuration.
 */
struct Series {
    QwtPlotCurve* curve = nullptr;  //!< Pointer to the Qwt curve object.
    CurveConfig config;                   //!< Style and rendering configuration.
    std::vector<double> x_data;     //!< Buffer for X-axis coordinates.
    std::vector<double> y_data;     //!< Buffer for Y-axis coordinates.
};

/*!
 * \brief The FileType enum The type of file to save
 */
enum FileType {
    CSV,   //!< Comma Seperated Value
    IMAGE, //!< PNG
};

/*!
 * \brief Defines how the plot behaves as new data points are added.
 */
enum PlotMode {
    CUMULATIVE, //!< Shows all data, expanding the X-axis as needed.
    ROLLING,    //!< Maintains a fixed window width, shifting with new data.
    SWEEP,      //!< Clears/re-draws from left to right in discrete pages.
};
} //namespace Plot


#endif //PLOT_DEFS_H
