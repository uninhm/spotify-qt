#pragma once

#include "enum/maskshape.hpp"

#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QVariant>

class ImageUtils
{
public:
	/** Mask image using specified shape */
	static auto mask(const QPixmap &source, MaskShape shape = MaskShape::App,
		const QVariant &data = QVariant()) -> QPixmap;

private:
	ImageUtils() = default;

	/** Get shape for MaskShape::App */
	static auto appShape(const QImage &img) -> QPolygonF;

	/** Get shape for MaskShape::Pie */
	static auto pieShape(const QImage &img, const QVariant &data) -> QPolygonF;
};
