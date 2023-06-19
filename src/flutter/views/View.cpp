/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "View.h"
#include "Platform.h"
#include "core/View_p.h"
#include "core/layouting/Item_p.h"
#include "../Window.h"
#include "ViewWrapper.h"


using namespace KDDockWidgets;
using namespace KDDockWidgets::flutter;

#define PRINT_UNEXPECTED_CALL_MESSAGE \
    qWarning() << Q_FUNC_INFO << "Unexpected call" << m_controller << int(m_controller->type());

View::View(Core::Controller *controller, Core::ViewType type, Core::View *parent,
           Qt::WindowFlags)
    : Core::View(controller, type)
{
    m_minSize = Core::Item::hardcodedMinimumSize;
    m_maxSize = Core::Item::hardcodedMaximumSize;
    m_geometry = QRect(0, 0, 400, 400);

    setParent(parent);
    m_inCtor = false;
}

View::~View()
{
    // qDebug() << "~View_flutter" << this << int(type());
    m_inDtor = true;
    if (hasFocus())
        Platform::platformFlutter()->setFocusedView({});

    if (m_parentView) {
        setParent(nullptr);
    }
}

void View::setGeometry(QRect geo)
{
    if (geo != m_geometry) {
        m_geometry = geo;
        onGeometryChanged();
    }
}

void View::move(int x, int y)
{
    if (m_geometry.topLeft() != QPoint(x, y)) {
        m_geometry.moveTopLeft(QPoint(x, y));
        onGeometryChanged();
    }
}

bool View::close()
{
    // TODO: Ask flutter if we should close

    CloseEvent ev;
    d->requestClose(&ev);

    if (ev.isAccepted()) {
        setVisible(false);
        return true;
    }

    return false;
}

bool View::isVisible() const
{
    // No value means false
    if (!m_visible.value_or(false))
        return false;

    // Parents need to be visible as well
    return !m_parentView || m_parentView->isVisible();
}

void View::setVisible(bool is)
{
    if (!m_visible.has_value() || is != m_visible.value()) {
        m_visible = is;

        if (m_visible) {
            // Mimic QWidgets: Set children visible, unless they were explicitly hidden
            for (auto child : qAsConst(m_childViews)) {
                if (!child->isExplicitlyHidden()) {
                    child->setVisible(true);
                }
            }
        }

        if (m_parentView) {
            m_parentView->onChildVisibilityChanged(this);
        }
    }
}

bool View::isExplicitlyHidden() const
{
    return m_visible.has_value() && !m_visible.value();
}

void View::setSize(int w, int h)
{
    m_geometry.setSize(QSize(w, h));
    onGeometryChanged();
}

std::shared_ptr<Core::View> View::rootView() const
{
    if (m_parentView)
        return m_parentView->rootView();

    return const_cast<View *>(this)->asWrapper();
}

void View::enableAttribute(Qt::WidgetAttribute, bool)
{
}

bool View::hasAttribute(Qt::WidgetAttribute) const
{
    return false;
}

void View::setFlag(Qt::WindowType, bool)
{
}

Qt::WindowFlags View::flags() const
{
    return {};
}

QSize View::minSize() const
{
    return m_minSize;
}

QSize View::maxSizeHint() const
{
    return m_maxSize;
}

QRect View::geometry() const
{
    return m_geometry;
}

QRect View::normalGeometry() const
{
    return m_geometry;
}

void View::setNormalGeometry(QRect)
{
}

void View::setMaximumSize(QSize s)
{
    s = s.boundedTo(Core::Item::hardcodedMaximumSize);
    if (s != m_maxSize) {
        m_maxSize = s;
        d->layoutInvalidated.emit();
    }
}

void View::setWidth(int w)
{
    if (m_geometry.width() != w) {
        m_geometry.setWidth(w);
        onGeometryChanged();
    }
}

void View::setHeight(int h)
{
    if (m_geometry.height() != h) {
        m_geometry.setHeight(h);
        onGeometryChanged();
    }
}

void View::setFixedWidth(int w)
{
    // TODOm5 Support fixed width
    setWidth(w);
}

void View::setFixedHeight(int h)
{
    // TODOm5 Support fixed height
    setHeight(h);
}

void View::show()
{
    setVisible(true);
}

void View::hide()
{
    setVisible(false);
}

void View::updateGeometry()
{
}

void View::update()
{
}

void View::setParent(Core::View *parent)
{
    if (parent == m_parentView)
        return;

    auto oldParent = m_parentView;
    m_parentView = static_cast<View *>(parent);

    if (oldParent) {
        if (!oldParent->inDtor())
            oldParent->onChildRemoved(this);
        oldParent->m_childViews.erase(std::remove_if(oldParent->m_childViews.begin(), oldParent->m_childViews.end(),
                                                     [this](Core::View *v) {
                                                         return v->equals(this);
                                                     }),
                                      oldParent->m_childViews.end());
    }

    if (m_parentView) {
        if (!m_inCtor) {
            // When in ctor there's no ViewMixin yet. TODO: Check if we need to improve this

            // Tell dart there's a new child
            m_parentView->onChildAdded(this);
        }

        // Track it in C++
        m_parentView->m_childViews.append(this);

        if (!m_parentView->isVisible() && isExplicitlyHidden()) {
            // Mimic QtWidget. Parenting removes the explicit hidden attribute if the parent is not visible
            m_visible = std::nullopt;
        }
    } else {
        if (!m_inDtor) {
            // Mimic Qt and hide when unparenting
            setVisible(false);
        }
    }
}

void View::raiseAndActivate()
{
    raise();
    activateWindow();
}

void View::activateWindow()
{
}

void View::raise()
{
    if (isRootView()) {
        raiseWindow(this);
    } else {
        m_parentView->raiseChild(this);
    }
}

QVariant View::viewProperty(const char *) const
{
    return {};
}

bool View::isRootView() const
{
    return m_parentView == nullptr;
}

QPoint View::mapToGlobal(QPoint) const
{
    qWarning() << Q_FUNC_INFO << "Implemented in Dart";
    return {};
}

QPoint View::mapFromGlobal(QPoint) const
{
    qWarning() << Q_FUNC_INFO << "Implemented in Dart";
    return {};
}

QPoint View::mapTo(Core::View *other, QPoint pt) const
{
    if (!other)
        return {};

    if (other->equals(this))
        return pt;

    const QPoint global = mapToGlobal(pt);
    return other->mapFromGlobal(global);
}

void View::setWindowOpacity(double)
{
}

void View::setWindowTitle(const QString &)
{
}

void View::setWindowIcon(const Icon &)
{
}

bool View::isActiveWindow() const
{
    return false;
}

void View::showNormal()
{
}

void View::showMinimized()
{
}

void View::showMaximized()
{
}

bool View::isMinimized() const
{
    return {};
}

bool View::isMaximized() const
{
    return {};
}

std::shared_ptr<Core::Window> View::window() const
{
    auto window = new flutter::Window(rootView());

    return std::shared_ptr<Core::Window>(window);
}

std::shared_ptr<Core::View> View::childViewAt(QPoint localPos) const
{
    if (!isMounted())
        return nullptr;

    const QPoint globalPt = mapToGlobal(localPos);

    for (auto child : m_childViews) {
        // Needs to be mounted (i.e. being shown by flutter's render tree, otherwise there's no geometry)
        if (!child->isVisible() || !static_cast<flutter::View *>(child)->isMounted())
            continue;

        if (auto result = child->childViewAt(child->mapFromGlobal(globalPt))) {
            return result;
        }

        // We favored depth first, but now it's our turn
        if (d->rect().contains(localPos))
            return const_cast<flutter::View *>(this)->asWrapper();
    }

    return nullptr;
}

std::shared_ptr<Core::View> View::parentView() const
{
    if (m_parentView)
        return m_parentView->asWrapper();

    return {};
}

std::shared_ptr<Core::View> View::asWrapper()
{
    return ViewWrapper::create(this);
}

void View::setViewName(const QString &name)
{
    m_name = name;
}

void View::grabMouse()
{
}

void View::releaseMouse()
{
}

void View::releaseKeyboard()
{
}

void View::setFocus(Qt::FocusReason)
{
    Platform::platformFlutter()->setFocusedView(asWrapper());
}

bool View::hasFocus() const
{
    auto focusedView = Platform::platformFlutter()->focusedView();
    return focusedView && focusedView->equals(this);
}

Qt::FocusPolicy View::focusPolicy() const
{
    return {};
}

void View::setFocusPolicy(Qt::FocusPolicy)
{
}

QString View::viewName() const
{
    return m_name;
}

void View::setMinimumSize(QSize s)
{
    s = s.expandedTo(Core::Item::hardcodedMinimumSize);
    if (s != m_minSize) {
        m_minSize = s;
        d->layoutInvalidated.emit();
    }
}

void View::render(QPainter *)
{
}

void View::setCursor(Qt::CursorShape)
{
}

void View::setMouseTracking(bool)
{
}

QVector<std::shared_ptr<Core::View>> View::childViews() const
{
    QVector<std::shared_ptr<Core::View>> children;
    children.reserve(m_childViews.size());
    for (auto child : m_childViews)
        children.append(ViewWrapper::create(static_cast<flutter::View *>(child)));

    return children;
}

void View::setZOrder(int)
{
}

HANDLE View::handle() const
{
    return this;
}

bool View::onFlutterWidgetResized(int w, int h)
{
    setSize(w, h);
    return Core::View::onResize(w, h);
}

void View::onChildAdded(Core::View *childView)
{
    Q_UNUSED(childView);
    dumpDebug();
    qFatal("Derived class should be called instead");
}

void View::onChildRemoved(Core::View *childView)
{
    Q_UNUSED(childView);
    dumpDebug();
    qFatal("Derived class should be called instead");
}

void View::onChildVisibilityChanged(Core::View *childView)
{
    Q_UNUSED(childView);
    dumpDebug();
    qFatal("Derived class should be called instead");
}

void View::onGeometryChanged()
{
    dumpDebug();
    qFatal("Derived class should be called instead");
}

void View::raiseChild(Core::View *)
{
    dumpDebug();
    qFatal("Derived class should be called instead");
}

void View::raiseWindow(Core::View *)
{
    dumpDebug();
    qFatal("Derived class should be called instead");
}

void View::onMouseEvent(Event::Type eventType, QPoint localPos, QPoint globalPos, bool leftIsPressed)
{
    // qDebug() << eventType << localPos << globalPos << leftIsPressed;
    Qt::MouseButtons buttons = Qt::NoButton;
    buttons.setFlag(Qt::LeftButton, leftIsPressed);
    Qt::KeyboardModifiers modifiers = Qt::NoModifier;

    if (eventType == Event::MouseMove) {
        // Poor man's QCursor::pos()
        flutter::Platform::s_lastCursorPosition = globalPos;
    }

    auto me = new MouseEvent(eventType, localPos, globalPos, globalPos, buttons, buttons, modifiers);

    if (!deliverViewEventToFilters(me)) {
        // filters allowed the event to propagate, give it to the view.
        // only pressed handled, there's no use case for the others
        if (eventType == Event::Type::MouseButtonPress)
            onMousePress(me);
    }

    // TODOm3: Who deletes the event ?
}

bool View::isMounted() const
{
    qWarning() << Q_FUNC_INFO << "Implemented in dart instead";
    return false;
}


void View::onRebuildRequested()
{
    qWarning() << Q_FUNC_INFO << "Implemented in dart instead";
}
