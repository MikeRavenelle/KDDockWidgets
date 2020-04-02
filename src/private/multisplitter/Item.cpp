/*
  This file is part of KDDockWidgets.

  Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Item_p.h"

#include <QDebug>
#include <QScopedValueRollback>

using namespace Layouting;

ItemContainer *Item::root() const
{
    return m_parent ? m_parent->root()
                    : const_cast<ItemContainer*>(qobject_cast<const ItemContainer*>(this));
}

void Item::resize(QSize newSize)
{
    setSize(newSize);
}

QSize Item::missingSize() const
{
    QSize missing = minSize() - this->size();
    missing.setWidth(qMax(missing.width(), 0));
    missing.setHeight(qMax(missing.height(), 0));

    return missing;
}

bool Item::isBeingInserted() const
{
    return m_sizingInfo.isBeingInserted;
}

void Item::setBeingInserted(bool is)
{
    m_sizingInfo.isBeingInserted = is;
}

void Item::setParentContainer(ItemContainer *parent)
{
    if (parent != m_parent) {
        if (m_parent) {
            disconnect(this, &Item::minSizeChanged, m_parent, &ItemContainer::onChildMinSizeChanged);
            disconnect(this, &Item::visibleChanged, m_parent, &ItemContainer::onChildVisibleChanged);
        }

        m_parent = parent;

        if (parent) {
            connect(this, &Item::minSizeChanged, parent, &ItemContainer::onChildMinSizeChanged);
            connect(this, &Item::visibleChanged, m_parent, &ItemContainer::onChildVisibleChanged);
        }

        QObject::setParent(parent);
    }
}

ItemContainer *Item::parentContainer() const
{
    return m_parent;
}

const ItemContainer *Item::asContainer() const
{
    Q_ASSERT(isContainer());
    return static_cast<const ItemContainer*>(this);
}

ItemContainer *Item::asContainer()
{
    return qobject_cast<ItemContainer*>(this);
}

void Item::setMinSize(QSize sz)
{
    Q_ASSERT(!isContainer());
    m_sizingInfo.minSize = sz;
}

void Item::setMaxSize(QSize sz)
{
    Q_ASSERT(!isContainer());
    m_sizingInfo.maxSize = sz;
}

QSize Item::minSize() const
{
    return m_sizingInfo.minSize;
}

QSize Item::maxSize() const
{
    return m_sizingInfo.maxSize;
}

void Item::setPos(QPoint pos)
{
    QRect geo = m_geometry;
    geo.moveTopLeft(pos);
    setGeometry(geo);
}

void Item::setPos(int pos, Qt::Orientation o)
{
    if (o == Qt::Vertical) {
        setPos({ x(), pos });
    } else {
        setPos({ pos, y() });
    }
}

int Item::pos(Qt::Orientation o) const
{
    return o == Qt::Vertical ? y() : x();
}

void Item::insertItem(Item *item, Location loc, SizingOption sizingOption)
{
    Q_ASSERT(item != this);
    const bool locIsSide1 = locationIsSide1(loc);

    if (sizingOption == SizingOption::Calculate)
        item->setGeometry(m_parent->suggestedDropRect(item, this, loc));

    if (m_parent->hasOrientationFor(loc)) {
        int indexInParent = m_parent->indexOfChild(this);
        if (!locIsSide1)
            indexInParent++;
        m_parent->insertItem(item, indexInParent);
    } else {
        ItemContainer *container = m_parent->convertChildToContainer(this);
        container->insertItem(item, loc, SizingOption::UseProvided);
    }
}

int Item::x() const
{
    return m_geometry.x();
}

int Item::y() const
{
    return m_geometry.y();
}

int Item::width() const
{
    return m_geometry.width();
}

int Item::height() const
{
    return m_geometry.height();
}

QSize Item::size() const
{
    return m_geometry.size();
}

void Item::setSize(QSize sz)
{
    QRect newGeo = m_geometry;
    newGeo.setSize(sz);
    setGeometry(newGeo);
}

QPoint Item::pos() const
{
    return m_geometry.topLeft();
}

int Item::position(Qt::Orientation o) const
{
    return o == Qt::Vertical ? y()
                             : x();
}

QRect Item::geometry() const
{
    return isBeingInserted() ? QRect()
                             : m_geometry;
}

bool Item::isContainer() const
{
    return m_isContainer;
}

Qt::Orientation Item::orientation() const
{
    return m_orientation;
}

int Item::minLength(Qt::Orientation o) const
{
    return Layouting::length(minSize(), o);
}

void Item::setLength(int length, Qt::Orientation o)
{
    Q_ASSERT(length > 0);
    if (o == Qt::Vertical)
        setSize({ width(), length });
    else
        setSize({ length, height() });
}

int Item::length(Qt::Orientation o) const
{
    return Layouting::length(size(), o);
}

int Item::availableLength(Qt::Orientation o) const
{
    return length(o) - minLength(o);
}

bool Item::isPlaceholder() const
{
    return !m_isVisible;
}

bool Item::isVisible() const
{
    return m_isVisible;
}

void Item::setIsVisible(bool is)
{
    if (is != m_isVisible) {
        m_isVisible = is;
        Q_EMIT minSizeChanged(this); // min-size is 0x0 when hidden
        Q_EMIT visibleChanged(this, is);
    }
}

void Item::setGeometry_recursive(QRect rect)
{
    // Recursiveness doesn't apply for for non-container items
    setGeometry(rect);
}

Item *Item::neighbour(Side side) const
{
    return m_parent ? m_parent->neighbourFor(this, side)
                    : nullptr;
}

int Item::separatorThickness()
{
    return 5;
}

bool Item::checkSanity() const
{
    if (minSize().width() > width() || minSize().height() > height()) {
        qWarning() << Q_FUNC_INFO << "Size constraints not honoured"
                   << "; min=" << minSize() << "; size=" << size();
        return false;
    }

    return true;
}

void Item::setGeometry(QRect rect)
{
    if (rect != m_geometry) {
        const QRect oldGeo = m_geometry;
        m_geometry = rect;

        Q_ASSERT(rect.width() > 0);
        Q_ASSERT(rect.height() > 0);

        Q_EMIT geometryChanged();

        if (oldGeo.x() != x())
            Q_EMIT xChanged();
        if (oldGeo.y() != y())
            Q_EMIT yChanged();
        if (oldGeo.width() != width())
            Q_EMIT widthChanged();
        if (oldGeo.height() != height())
            Q_EMIT heightChanged();
    }
}

void Item::dumpLayout(int level)
{
    QString indent;
    indent.fill(QLatin1Char(' '), level);
    const QString beingInserted = m_sizingInfo.isBeingInserted ? QStringLiteral(";beingInserted;")
                                                               : QString();
    const QString visible = !isVisible() ? QStringLiteral(";hidden;")
                                         : QString();
    qDebug().noquote() << indent << "- Widget: " << objectName() << m_geometry << visible << beingInserted;
}

Item::Item(ItemContainer *parent)
    : QObject(parent)
    , m_isContainer(false)
    , m_parent(parent)
{
}

Item::Item(bool isContainer, ItemContainer *parent)
    : QObject(parent)
    , m_isContainer(isContainer)
    , m_parent(parent)
{
}

bool Item::isRoot() const
{
    return m_parent == nullptr;
}

bool Item::isVertical() const
{
    return m_orientation == Qt::Vertical;
}

bool Item::isHorizontal() const
{
    return m_orientation == Qt::Horizontal;
}

int Item::availableOnSide(Side, Qt::Orientation o) const
{
    if (isRoot())
        return 0;

    ItemContainer *container = parentContainer();

    if (o == container->orientation()) {

    } else {

    }
    return 0;
}

ItemContainer::ItemContainer(ItemContainer *parent)
    : Item(true, parent)
{
    connect(this, &Item::xChanged, this, [this] {
        for (Item *item : qAsConst(m_children)) {
            Q_EMIT item->xChanged();
        }
    });

    connect(this, &Item::yChanged, this, [this] {
        for (Item *item : qAsConst(m_children)) {
            Q_EMIT item->yChanged();
        }
    });
}

bool ItemContainer::checkSanity() const
{
    if (!Item::checkSanity())
        return false;

    if (numChildren() == 0 && !isRoot()) {
        qWarning() << Q_FUNC_INFO << "Container is empty. Should be deleted";
        return false;
    }

    // Check that the geometries don't overlap
    int expectedPos = Layouting::pos(pos(), m_orientation);
    for (Item *item : m_children) {
        const int pos = Layouting::pos(item->pos(), m_orientation);
        if (expectedPos != pos) {
            qWarning() << Q_FUNC_INFO << "Unexpected pos" << pos << "; expected=" << expectedPos
                       << "; for item=" << item
                       << "; isContainer=" << item->isContainer();
            return false;
        }

        expectedPos = pos + Layouting::length(item->size(), m_orientation) + separatorThickness();
    }

    const int h1 = Layouting::length(size(), oppositeOrientation(m_orientation));
    for (Item *item : m_children) { 
        if (item->parentContainer() != this) {
            qWarning() << "Invalid parent container for" << item
                       << "; is=" << item->parentContainer() << "; expected=" << this;
            return false;
        }

        if (item->parent() != this) {
            qWarning() << "Invalid QObject parent for" << item
                       << "; is=" << item->parent() << "; expected=" << this;
            return false;
        }

        // Check the children height (if horizontal, and vice-versa)
        const int h2 = Layouting::length(item->size(), oppositeOrientation(m_orientation));
        if (h1 != h2) {
            qWarning() << Q_FUNC_INFO << "Invalid size for item." << item
                       << "Container.length=" << h1 << "; item.length=" << h2;
            root()->dumpLayout();
            return false;
        }

        if (!rect().contains(item->geometry())) {
            qWarning() << Q_FUNC_INFO << "Item geo is out of bounds. item=" << item << "; geo="
                       << item->geometry() << "; container.rect=" << rect();
            root()->dumpLayout();
            return false;
        }

        if (!item->checkSanity())
            return false;
    }

    return true;
}

bool ItemContainer::hasOrientation() const
{
    return isVertical() || isHorizontal();
}

int ItemContainer::numChildren() const
{
    return m_children.size();
}

int ItemContainer::numVisibleChildren() const
{
    int num = 0;
    for (Item *child : m_children) {
        if (child->isVisible())
            num++;
    }
    return num;
}

int ItemContainer::indexOfChild(const Item *item) const
{
    return m_children.indexOf(const_cast<Item *>(item));
}

void ItemContainer::removeItem(Item *item)
{
    Q_ASSERT(!item->isRoot());
    if (contains(item)) {
        Item *side1Item = visibleNeighbourFor(item, Side1);
        Item *side2Item = visibleNeighbourFor(item, Side2);
        const bool removed = m_children.removeOne(item);
        Q_ASSERT(removed);
        delete item;
        m_childPercentages.clear();
        if (isEmpty() && !isRoot()) {
            parentContainer()->removeItem(this);
        } else {
            // Neighbours will occupy the space of the deleted item
            growNeighbours(side1Item, side2Item);
            Q_EMIT itemsChanged();
        }
    } else {
        item->parentContainer()->removeItem(item);
    }
}

bool ItemContainer::isEmpty() const
{
    return m_children.isEmpty();
}

void ItemContainer::setGeometry_recursive(QRect rect)
{
    setPos(rect.topLeft());

    // Call resize, which is recursive and will resize the children too
    resize(rect.size());
}

ItemContainer *ItemContainer::convertChildToContainer(Item *leaf)
{
    const int index = indexOfChild(leaf);
    Q_ASSERT(index != -1);
    auto container = new ItemContainer(this);
    insertItem(container, index, /*growItem=*/false);
    m_children.removeOne(leaf);
    container->setGeometry(leaf->geometry());
    container->insertItem(leaf, Location_OnTop, SizingOption::UseProvided);
    Q_EMIT itemsChanged();

    return container;
}

void ItemContainer::insertItem(Item *item, Location loc, SizingOption sizingOption)
{
    item->setIsVisible(false);

    if (isRoot() && sizingOption == SizingOption::Calculate)
        item->setGeometry(suggestedDropRect(item, nullptr, loc));

    Q_ASSERT(item != this);
    if (contains(item)) {
        qWarning() << Q_FUNC_INFO << "Item already exists";
        return;
    }

    const Qt::Orientation locOrientation = orientationForLocation(loc);

    if (hasOrientationFor(loc)) {
        if (m_children.size() == 1) {
            // 2 items is the minimum to know which orientation we're layedout
            m_orientation = locOrientation;
        }

        const int index = locationIsSide1(loc) ? 0 : m_children.size();
        insertItem(item, index);
    } else {
        // Inserting directly in a container ? Only if it's root.
        Q_ASSERT(isRoot());
        auto container = new ItemContainer(this);
        container->setChildren(m_children);
        container->m_orientation = m_orientation;
        m_children.clear();
        m_orientation = oppositeOrientation(m_orientation);
        insertItem(container, 0, /*grow=*/ false);
        container->setGeometry(rect());
        container->setIsVisible(container->numVisibleChildren() > 0);

        // Now we have the correct orientation, we can insert
        insertItem(item, loc);
    }
}

void ItemContainer::onChildMinSizeChanged(Item *child)
{
    const QSize missingSize = this->missingSize();
    if (!missingSize.isNull()) {
        QScopedValueRollback<bool> resizing(m_isResizing, true);

        if (isRoot()) {
            // Resize the whole layout
            resize(size() + missingSize);
            Item::List childs = visibleChildren();
            Item *lastChild = nullptr;
            for (int i = childs.size() - 1; i >= 0; i--) {
                if (!childs[i]->isBeingInserted()) {
                    lastChild = childs[i];
                    break;
                }
            }

            if (lastChild) {
                QRect r = lastChild->geometry();
                r.adjust(0, 0, missingSize.width(), missingSize.height());
                lastChild->setGeometry(r);
            }
        }

        // Our min-size changed, notify our parent, and so on until it reaches root()
        Q_EMIT minSizeChanged(this);
    }

    if (numVisibleChildren() == 1) {
        // The easy case. Child is alone in the layout, occupies everything.
        child->setGeometry(rect());
        return;
    }

    const QSize missingForChild = child->missingSize();
    if (missingForChild.isNull()) {
        // The child changed its minSize. Thanks for letting us know, but there's nothing needed doing.
        // Item is bigger than its minimum.
        //Q_ASSERT(false); // Never happens I think. Remove this if!
        return;
    }

    // Child has some growing to do. It will grow left and right equally, (and top-bottom), as needed.
    growItem(child, Layouting::length(missingForChild, m_orientation), GrowthStrategy::BothSidesEqually);
}

void ItemContainer::onChildVisibleChanged(Item *child, bool visible)
{
    if (visible != isVisible()) {
        if (visible)
            setIsVisible(true);
        else
            setIsVisible(numVisibleChildren() > 0);
    }

    if (!visible)
        return;

    return;

    if (numVisibleChildren() == 1) {
        // There's no separator when there's only 1
        return;
    }

    // Shift the neighbours by 5px, to make space for the separator
    const int available1 = availableOnSide(child, Side1);
    const int available2 = availableOnSide(child, Side2);
    int needed = Item::separatorThickness();

    if (available1 > 0) { // The available squeeze on the left (or top)
        Item *neighbour1 = visibleNeighbourFor(child, Side1);
        Q_ASSERT(neighbour1);
        const int side1Shift = qMin(needed, available1);
        needed -= side1Shift;

        const QRect geo1 = adjustedRect(neighbour1->geometry(), m_orientation, 0, -side1Shift);
        neighbour1->setGeometry_recursive(geo1);
    }

    if (needed > 0) { // Squeeze on the right, if needed
        Item *neighbour2 = visibleNeighbourFor(child, Side2);
        Q_ASSERT(neighbour2);
        Q_ASSERT(available2 >= needed);
        const QRect geo2 = adjustedRect(neighbour2->geometry(), m_orientation, 0, -needed);
        neighbour2->setGeometry_recursive(geo2);
    }
}

QRect ItemContainer::suggestedDropRect(Item *newItem, Item *relativeTo, Location loc) const
{
    Q_ASSERT(newItem);
    if (relativeTo && !relativeTo->parentContainer()) {
        qWarning() << Q_FUNC_INFO << "No parent container";
        return {};
    }

    if (relativeTo && relativeTo->parentContainer() != this) {
        qWarning() << Q_FUNC_INFO << "Called on the wrong container";
        return {};
    }

    if (loc == Location_None) {
        qWarning() << Q_FUNC_INFO << "Invalid location";
        return {};
    }
    const int itemMin = newItem->minLength(m_orientation);
    const int available = availableLength() - Item::separatorThickness();

    if (relativeTo) {
        const int equitativeLength = usableLength() / (m_children.size() + 1);
        const int suggestedLength = qMax(qMin(available, equitativeLength), itemMin);
        const int indexOfRelativeTo = indexOfChild(relativeTo);

        //int availableSide1 = 0;
        //int min1 = 0; TODO
        //int max2 = 0;

        //const int availableSide2 = availableOnSide(m_children.at(indexOfRelativeTo), Side2);
        //const int relativeToPos = relativeTo->position(m_orientation);
        if (locationIsSide1(loc)) {
            if (indexOfRelativeTo > 0) {
                //availableSide1 = availableOnSide(m_children.at(indexOfRelativeTo - 1), Side1);
                // min1 = relativeToPos - availableSide1; TODO
                // max2 = relativeToPos + availableSide2;
            }
        } else {
            if (indexOfRelativeTo < m_children.size() - 1) {
                //availableSide1 = availableOnSide(m_children.at(indexOfRelativeTo + 1), Side1);
                // min1 = relativeToPos + relativeTo->length(m_orientation) - availableSide1; TODO
                // max2 = relativeToPos + relativeTo->length(m_orientation) + availableSide2;
            }
        }

        QRect rect;

        if (orientationForLocation(loc) == Qt::Vertical) {
            rect.setX(0);
            rect.setSize(QSize(relativeTo->width(), suggestedLength));
        } else {
            rect.setY(0);
            rect.setSize(QSize(suggestedLength, relativeTo->height()));
        }

        return rect;

    } else if (isRoot()) {
        // Relative to the window itself
        QRect rect = this->rect();
        const int oneThird = length() / 3;
        const int suggestedLength = qMax(qMin(available, oneThird), itemMin);

        switch (loc) {
        case Location_OnLeft:
            rect.setWidth(suggestedLength);
            break;
        case Location_OnTop:
            rect.setHeight(suggestedLength);
            break;
        case Location_OnRight:
            rect.adjust(rect.width() - suggestedLength, 0, 0, 0);
            break;
        case Location_OnBottom:
            rect.adjust(0, rect.bottom() - suggestedLength, 0, 0);
            break;
        case Location_None:
            return {};
        }

        return rect;

    } else {
        qWarning() << Q_FUNC_INFO << "Shouldn't happen";
    }

    return {};
}

void ItemContainer::positionItems()
{
    Item::List children = visibleChildren();
    int nextPos = 0;
    const Qt::Orientation oppositeOrientation = Layouting::oppositeOrientation(m_orientation);
    for (int i = 0; i < children.size(); ++i) {
        Item *item = children.at(i);

        // If the layout is horizontal, the item will have the height of the container. And vice-versa
        const int oppositeLength = Layouting::length(size(), oppositeOrientation);
        item->setLength(oppositeLength, oppositeOrientation);

        // Update the pos
        item->setPos(nextPos, m_orientation);
        nextPos += item->length(m_orientation) + Item::separatorThickness();
    }

    m_childPercentages.clear();
}

void ItemContainer::clear()
{
    for (Item *item : qAsConst(m_children)) {
        if (ItemContainer *container = item->asContainer())
            container->clear();

        delete item;
    }
}

void ItemContainer::insertItem(Item *item, int index, bool growItem)
{
    m_children.insert(index, item);
    item->setParentContainer(this);
    m_childPercentages.clear();
    Q_EMIT itemsChanged();

    if (growItem)
        restorePlaceholder(item);
}

bool ItemContainer::hasChildren() const
{
    return !m_children.isEmpty();
}

bool ItemContainer::hasVisibleChildren() const
{
    for (Item *item : m_children)
        if (item->isVisible())
            return true;

    return false;
}

bool ItemContainer::hasOrientationFor(Location loc) const
{
    if (m_children.size() <= 1)
        return true;

    return m_orientation == orientationForLocation(loc);
}

Item::List ItemContainer::children() const
{
    return m_children;
}

Item::List ItemContainer::visibleChildren() const
{
    Item::List items;
    items.reserve(m_children.size());
    for (Item *item : m_children) {
        if (item->isVisible())
            items << item;
    }

    return items;
}

int ItemContainer::usableLength() const
{
    const int numVisibleChildren = this->numVisibleChildren();
    if (numVisibleChildren <= 1)
        return Layouting::length(size(), m_orientation);

    const int separatorWaste = separatorThickness() * (numVisibleChildren - 1);
    return length() - separatorWaste;
}

bool ItemContainer::hasSingleVisibleItem() const
{
    return numVisibleChildren() == 1;
}

bool ItemContainer::contains(Item *item) const
{
    return m_children.contains(item);
}

void ItemContainer::setChildren(const Item::List children)
{
    m_children = children;
    for (Item *item : children)
        item->setParentContainer(this);
}

QSize ItemContainer::minSize() const
{
    int minW = 0;
    int minH = 0;

    if (!isEmpty()) {
        const Item::List visibleChildren = this->visibleChildren();
        for (Item *item : visibleChildren) {
            if (isVertical()) {
                minW = qMax(minW, item->minSize().width());
                minH += item->minSize().height();
            } else {
                minH = qMax(minH, item->minSize().height());
                minW += item->minSize().width();
            }
        }

        const int separatorWaste = (visibleChildren.size() - 1) * separatorThickness();
        if (isVertical())
            minH += separatorWaste;
        else
            minW += separatorWaste;
    }

    return { minW, minH };
}

QSize ItemContainer::maxSize() const
{
    int maxW = 0;
    int maxH = 0;

    if (!isEmpty()) {
        const Item::List visibleChildren = this->visibleChildren();
        for (Item *item : visibleChildren) {
            if (isVertical()) {
                maxW = qMin(maxW, item->maxSize().width());
                maxH += item->maxSize().height();
            } else {
                maxH = qMin(maxH, item->maxSize().height());
                maxW += item->maxSize().width();
            }
        }

        const int separatorWaste = (visibleChildren.size() - 1) * separatorThickness();
        if (isVertical())
            maxH += separatorWaste;
        else
            maxW += separatorWaste;
    }

    return { maxW, maxH };
}

void ItemContainer::resize(QSize newSize)
{
    const QSize minSize = this->minSize();
    if (newSize.width() < minSize.width() || newSize.height() < minSize.height()) {
        qWarning() << Q_FUNC_INFO << "New size doesn't respect size constraints";
        return;
    }

    const bool widthChanged = width() != newSize.width();
    const bool heightChanged = height() != newSize.height();
    if (!widthChanged && !heightChanged)
        return;

    const bool lengthChanged = (isVertical() && heightChanged) || (isHorizontal() && widthChanged);

    if (m_childPercentages.isEmpty())
        updateChildPercentages();

    setSize(newSize);

    if (m_isResizing) {
        // We're already under a resize, nothing to do
        return;
    }

    const int totalNewLength = usableLength();
    int remaining = totalNewLength;

    int nextPos = 0;
    for (int i = 0, count = m_children.size(); i < count; ++i) {
        const bool isLast = i == count - 1;

        Item *item = m_children.at(i);
        const qreal childPercentage = m_childPercentages.at(i);
        const int newItemLength = lengthChanged ? (isLast ? remaining
                                                          : int(childPercentage * totalNewLength))
                                                : item->length(m_orientation);
        item->setPos(nextPos, m_orientation);
        nextPos += newItemLength + separatorThickness();
        remaining = remaining - newItemLength;

        if (isVertical()) {
            item->resize({ width(), newItemLength });
        } else {
            item->resize({ newItemLength, height() });
        }
    }
}

int ItemContainer::length() const
{
    return isVertical() ? height() : width();
}

QRect ItemContainer::rect() const
{
    QRect rect = m_geometry;
    rect.moveTo(QPoint(0, 0));
    return rect;
}

void ItemContainer::dumpLayout(int level)
{
    QString indent;
    indent.fill(QLatin1Char(' '), level);
    const QString beingInserted = m_sizingInfo.isBeingInserted ? QStringLiteral("; beingInserted;")
                                                               : QString();
    qDebug().noquote() << indent << "* Layout: " << m_orientation << m_geometry << "; this="
                       << this << beingInserted;
    for (Item *item : qAsConst(m_children)) {
        item->dumpLayout(level + 1);
    }
}

void ItemContainer::updateChildPercentages()
{
    m_childPercentages.clear();
    m_childPercentages.reserve(m_children.size());
    const int available = usableLength();
    for (Item *item : m_children)
        m_childPercentages << (1.0 * item->length(m_orientation)) / available;
}

void ItemContainer::restorePlaceholder(Item *item)
{
    Q_ASSERT(contains(item));

    item->setBeingInserted(true); // TODO: Move into setIsVisible ?
    item->setIsVisible(true);
    item->setBeingInserted(false);
    positionItems();

    if (numVisibleChildren() == 1)
        return;

    const int available = availableLength();
    const int maxItemLength = item->minLength(m_orientation) + available;
    const int proposedItemLength = item->length(m_orientation);
    const int newLength = proposedItemLength > maxItemLength ? maxItemLength
                                                             : proposedItemLength;
    item->setLength(newLength, m_orientation);
    growItem(item, newLength, GrowthStrategy::BothSidesEqually);
}

Item *ItemContainer::visibleNeighbourFor(const Item *item, Side side) const
{
    const Item::List children = visibleChildren();
    const int index = children.indexOf(const_cast<Item *>(item));
    const int neighbourIndex = side == Side1 ? index - 1
                                             : index + 1;

    return (neighbourIndex >= 0 && neighbourIndex < children.size()) ? children.at(neighbourIndex)
                                                                     : nullptr;
}

Item *ItemContainer::neighbourFor(const Item *item, Side side) const
{
    const int index = indexOfChild(item);
    const int neighbourIndex = side == Side1 ? index - 1
                                             : index + 1;

    return (neighbourIndex >= 0 && neighbourIndex < m_children.size()) ? m_children.at(neighbourIndex)
                                                                       : nullptr;
}

QSize ItemContainer::availableSize() const
{
    return size() - this->minSize();
}

int ItemContainer::availableLength() const
{
    return isVertical() ? availableSize().height()
                        : availableSize().width();
}

int ItemContainer::neighboursLengthFor(const Item *item, Side side, Qt::Orientation o) const
{
    const int index = indexOfChild(item);
    if (index == -1) {
        qWarning() << Q_FUNC_INFO << "Couldn't find item" << item;
        return 0;
    }

    if (o == m_orientation) {
        int neighbourLength = 0;
        int start = 0;
        int end = -1;
        if (side == Side1) {
            start = 0;
            end = index - 1;
        } else {
            start = index + 1;
            end = m_children.size() - 1;
        }

        for (int i = start; i <= end; ++i)
            neighbourLength += m_children.at(i)->length(m_orientation);

        return neighbourLength;
    } else {
        // No neighbours in the other orientation. Each container is bidimensional.
        return 0;
    }
}

int ItemContainer::neighboursLengthFor_recursive(const Item *item, Side side, Qt::Orientation o) const
{
    return neighboursLengthFor(item, side, o) + (isRoot() ? 0
                                                          : parentContainer()->neighboursLengthFor_recursive(this, side, o));

}

int ItemContainer::neighboursMinLengthFor(const Item *item, Side side, Qt::Orientation o) const
{
    const int index = indexOfChild(item);
    if (index == -1) {
        qWarning() << Q_FUNC_INFO << "Couldn't find item" << item;
        return 0;
    }

    if (o == m_orientation) {
        int neighbourMinLength = 0;
        int start = 0;
        int end = -1;
        if (side == Side1) {
            start = 0;
            end = index - 1;
        } else {
            start = index + 1;
            end = m_children.size() - 1;
        }

        for (int i = start; i <= end; ++i)
            neighbourMinLength += m_children.at(i)->minLength(m_orientation);

        return neighbourMinLength;
    } else {
        // No neighbours here
        return 0;
    }
}

int ItemContainer::neighboursMinLengthFor_recursive(const Item *item, Side side, Qt::Orientation o) const
{
    return neighboursMinLengthFor(item, side, o) + (isRoot() ? 0
                                                             : parentContainer()->neighboursMinLengthFor(this, side, o));
}

int ItemContainer::neighbourSeparatorWaste(const Item *item, Side side, Qt::Orientation o) const
{
    const int index = indexOfChild(item);
    if (index == -1) {
        qWarning() << Q_FUNC_INFO << "Couldn't find item" << item;
        return 0;
    }

    if (o == m_orientation) {
        if (side == Side1) {
            return index * Item::separatorThickness();
        } else {
            return (m_children.size() - 1 - index) * Item::separatorThickness();
        }
    } else {
        return 0;
    }
}

int ItemContainer::neighbourSeparatorWaste_recursive(const Item *item, Side side, Qt::Orientation orientation) const
{
    return neighbourSeparatorWaste(item, side, orientation) + (isRoot() ? 0
                                                                        : parentContainer()->neighbourSeparatorWaste(item, side, orientation));
}

int ItemContainer::availableOnSide(Item *child, Side side) const
{
    const int length = neighboursLengthFor(child, side, m_orientation);
    const int min = neighboursMinLengthFor(child, side, m_orientation);

    const int available = length - min;
    if (available < 0) {
        root()->dumpLayout();
        Q_ASSERT(false);
    }
    return available;
}

QSize ItemContainer::missingSizeFor(Item *item, Qt::Orientation o) const
{
    QSize missing = {0, 0};
    const QSize available = availableSize();
    const int anchorWasteW = (o == Qt::Vertical || !hasVisibleChildren()) ? 0 : Item::separatorThickness();
    const int anchorWasteH = (o == Qt::Vertical && hasVisibleChildren()) ? Item::separatorThickness() : 0;
    missing.setWidth(qMax(item->minSize().width() - available.width() + anchorWasteW, 0));
    missing.setHeight(qMax(item->minSize().height() - available.height() + anchorWasteH, 0));

    return missing;
}

QVariantList ItemContainer::items() const
{
    QVariantList items;
    items.reserve(m_children.size());

    for (auto item : m_children)
        items << QVariant::fromValue(item);

    return items;
}

void ItemContainer::growNeighbours(Item *side1Neighbour, Item *side2Neighbour)
{
    if (!side1Neighbour && !side2Neighbour)
        return;

    if (side1Neighbour && side2Neighbour) {
        // Give half/half to each neighbour
        QRect geo1 = side1Neighbour->geometry();
        QRect geo2 = side2Neighbour->geometry();

        if (isVertical()) {
            const int available = geo2.y() - geo1.bottom() - separatorThickness();
            geo1.setHeight(geo1.height() + available / 2);
            geo2.setTop(geo1.bottom() + separatorThickness() + 1);
        } else {
            const int available = geo2.x() - geo1.right() - separatorThickness();
            geo1.setWidth(geo1.width() + available / 2);
            geo2.setLeft(geo1.right() + separatorThickness() + 1);
        }

        side1Neighbour->setGeometry_recursive(geo1);
        side2Neighbour->setGeometry_recursive(geo2);

    } else if (side1Neighbour) {
        // Grow all the way to the right (or bottom if vertical)
        QRect geo = side1Neighbour->geometry();

        if (isVertical()) {
            geo.setBottom(rect().bottom());
        } else {
            geo.setRight(rect().right());
        }

        side1Neighbour->setGeometry_recursive(geo);

    } else if (side2Neighbour) {
        // Grow all the way to the left (or top if vertical)
        QRect geo = side2Neighbour->geometry();

        if (isVertical()) {
            geo.setTop(0);
        } else {
            geo.setLeft(0);
        }

        side2Neighbour->setGeometry_recursive(geo);
    }
}

void ItemContainer::growItem(Item *item, int amount, GrowthStrategy growthStrategy)
{
    if (amount == 0)
        return;

    Q_ASSERT(growthStrategy == GrowthStrategy::BothSidesEqually);
    const Item::List visibleItems = visibleChildren();
    const int index = visibleItems.indexOf(item);

    if (visibleItems.size() == 1) {
        Q_ASSERT(index != -1);
        //There's no neighbours to push, we're alone. Occupy the full container
        item->setLength(item->length(m_orientation) + amount, m_orientation);
        positionItems();
        return;
    }

    const int available1 = availableOnSide(item, Side1);
    const int available2 = availableOnSide(item, Side2);
    const int neededLength = amount;

    int min1 = 0;
    int max2 = length() - 1;
    int newPosition = 0;
    int side1Growth = 0;

    Item *side1Neighbour = index > 0 ? visibleItems.at(index - 1)
                                     : nullptr;

    if (side1Neighbour) {
        Item *side1Neighbour = visibleItems.at(index - 1);
        min1 = side1Neighbour->position(m_orientation) + side1Neighbour->length(m_orientation) - available1;
        newPosition = side1Neighbour->position(m_orientation) + side1Neighbour->length(m_orientation) - (amount / 2);
    }

    if (index < visibleItems.size() - 1) {
        max2 = visibleItems.at(index + 1)->position(m_orientation) + available2;
    }

    // Now bound the position
    if (newPosition < min1) {
        newPosition = min1;
    } else if (newPosition + neededLength > max2) {
        newPosition = max2 - neededLength - Item::separatorThickness() + 1;
    }

    if (newPosition > 0) {
        side1Growth = side1Neighbour->position(m_orientation) + side1Neighbour->length(m_orientation) - newPosition;
    }

    const int side2Growth = neededLength - side1Growth + Item::separatorThickness();
    growItem(item, side1Growth, side2Growth);
}

QVector<int> ItemContainer::availableLengthPerNeighbour(Item *item, Side side) const
{
    Item::List children = visibleChildren();
    const int indexOfChild = children.indexOf(item);

    int start = 0;
    int end = 0;
    if (side == Side1) {
        start = 0;
        end = indexOfChild - 1;
    } else {
        start = indexOfChild + 1;
        end = children.size() - 1;
    }

    QVector<int> result;
    result.reserve(end - start + 1);
    for (int i = start; i <= end; ++i) {
        Item *neighbour = children.at(i);
        result << neighbour->availableLength(m_orientation);
    }

    return result;
}

/** static */
QVector<int> ItemContainer::calculateSqueezes(QVector<int> availabilities, int needed)
{
    QVector<int> squeezes(availabilities.size(), 0);
    int missing = needed;
    while (missing > 0) {
        const int numDonors = std::count_if(availabilities.cbegin(), availabilities.cend(), [] (int num) {
            return num > 0;
        });

        if (numDonors == 0) {
            Q_ASSERT(false);
            return {};
        }

        int toTake = missing / numDonors;
        if (toTake == 0)
            toTake = missing;

        for (int i = 0; i < availabilities.size(); ++i) {
            const int available = availabilities.at(i);
            if (available == 0)
                continue;
            const int took = qMin(toTake, availabilities.at(i));
            availabilities[i] -= took;
            missing -= took;
            squeezes[i] += took;
            if (missing == 0)
                break;
        }
    }

    return squeezes;
}

void ItemContainer::growItem(Item *child, int side1Growth, int side2Growth)
{
    Q_ASSERT(side1Growth > 0 || side2Growth > 0);

    Item::List children = visibleChildren();

    if (side1Growth > 0) {
        const QVector<int> availables = availableLengthPerNeighbour(child, Side1);
        const QVector<int> squeezes = calculateSqueezes(availables, side1Growth);
        for (int i = 0; i < squeezes.size(); ++i) {
            const int squeeze = squeezes.at(i);
            Item *neighbour = children[i];
            neighbour->setGeometry_recursive(adjustedRect(neighbour->geometry(), m_orientation, 0, -squeeze));
        }
    }

    if (side2Growth > 0) {
        const QVector<int> availables = availableLengthPerNeighbour(child, Side2);
        const int itemIndex = children.indexOf(child);
        const QVector<int> squeezes = calculateSqueezes(availables, side2Growth);
        for (int i = 0; i < squeezes.size(); ++i) {
            const int squeeze = squeezes.at(i);
            Item *neighbour = children[i + itemIndex + 1];
            neighbour->setGeometry_recursive(adjustedRect(neighbour->geometry(), m_orientation, squeeze, 0));
        }
    }

    positionItems();
}
