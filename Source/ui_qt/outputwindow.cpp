#include "outputwindow.h"
#include <QResizeEvent>
#include <QTimer>
#include <cstdlib>

// Arcade cabinet: PLAY_GUN_CROSSHAIR=1 (set by the launcher for light-gun games) keeps a crosshair cursor visible
// in fullscreen instead of hiding it, so trackball players can see where they aim.
static bool GunCrosshairEnabled()
{
	const char* value = getenv("PLAY_GUN_CROSSHAIR");
	return value && (value[0] == '1');
}

OutputWindow::OutputWindow(QWindow* parent)
    : QWindow(parent)
{
	m_fullScreenCursorTimer = new QTimer(this);
	m_fullScreenCursorTimer->setSingleShot(true);
	connect(m_fullScreenCursorTimer, &QTimer::timeout, [this]() { setCursor(GunCrosshairEnabled() ? Qt::CrossCursor : Qt::BlankCursor); });
	connect(this, SIGNAL(activeChanged()), this, SLOT(activeStateChanged()));
}

void OutputWindow::ShowFullScreenCursor()
{
	m_fullScreenCursorTimer->start(2000);
	m_fullScreenCursorActive = true;
	setCursor(GunCrosshairEnabled() ? Qt::CrossCursor : Qt::ArrowCursor);
}

void OutputWindow::DismissFullScreenCursor()
{
	m_fullScreenCursorTimer->stop();
	m_fullScreenCursorActive = false;
	setCursor(GunCrosshairEnabled() ? Qt::CrossCursor : Qt::ArrowCursor);
}

void OutputWindow::keyPressEvent(QKeyEvent* ev)
{
	emit keyDown(ev);
}

void OutputWindow::keyReleaseEvent(QKeyEvent* ev)
{
	emit keyUp(ev);
}

void OutputWindow::exposeEvent(QExposeEvent* ev)
{
	if(!ev->region().isNull())
	{
		emit widthChanged(size().width());
	}
	QWindow::exposeEvent(ev);
}

void OutputWindow::focusOutEvent(QFocusEvent* event)
{
	emit focusOut(event);
}

void OutputWindow::focusInEvent(QFocusEvent* event)
{
	emit focusIn(event);
}

void OutputWindow::activeStateChanged()
{
	if(isActive())
	{
		emit focusIn(new QFocusEvent(QEvent::FocusIn));
	}
	else
	{
		emit focusOut(new QFocusEvent(QEvent::FocusOut));
	}
}

void OutputWindow::mouseMoveEvent(QMouseEvent* ev)
{
	if(m_fullScreenCursorActive)
	{
		ShowFullScreenCursor();
	}
	emit mouseMove(ev);
}

void OutputWindow::mousePressEvent(QMouseEvent* ev)
{
	emit mousePress(ev);
}

void OutputWindow::mouseReleaseEvent(QMouseEvent* ev)
{
	emit mouseRelease(ev);
}

void OutputWindow::mouseDoubleClickEvent(QMouseEvent* ev)
{
	emit doubleClick(ev);
}

void OutputWindow::wheelEvent(QWheelEvent* ev)
{
	emit mouseWheel(ev);
}
