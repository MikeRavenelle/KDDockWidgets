#
# This file is part of KDDockWidgets.
#
# SPDX-FileCopyrightText: 2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Sergio Martins <sergio.martins@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

set(KDDW_LAYOUTING_SRCS ../Item.cpp)

add_library(kddockwidgetslayouting ${KDDW_LAYOUTING_SRCS})

target_include_directories(
    kddockwidgetslayouting PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../fwd_headers>
                                  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

target_link_libraries(kddockwidgetslayouting PRIVATE kdbindings)

link_to_nlohman(kddockwidgetslayouting)

target_compile_definitions(kddockwidgetslayouting PRIVATE KDDW_RUST_LAYOUTING_ENGINE)

include(GenerateExportHeader)
generate_export_header(
    kddockwidgetslayouting EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/kddockwidgets_export.h" EXPORT_MACRO_NAME DOCKS_EXPORT
)
