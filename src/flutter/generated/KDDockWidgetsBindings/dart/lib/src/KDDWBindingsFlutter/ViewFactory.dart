/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
import 'dart:ffi' as ffi;
import 'package:ffi/ffi.dart';
import '../TypeHelpers.dart';
import '../../Bindings.dart';
import '../../Bindings_KDDWBindingsCore.dart' as KDDWBindingsCore;
import '../../Bindings_KDDWBindingsFlutter.dart' as KDDWBindingsFlutter;
import '../../LibraryLoader.dart';

var _dylib = Library.instance().dylib;

class ViewFactory extends KDDWBindingsCore.ViewFactory {
  ViewFactory.fromCppPointer(var cppPointer, [var needsAutoDelete = false])
      : super.fromCppPointer(cppPointer, needsAutoDelete) {}
  ViewFactory.init() : super.init() {}
  factory ViewFactory.fromCache(var cppPointer, [needsAutoDelete = false]) {
    if (KDDWBindingsCore.Object.isCached(cppPointer)) {
      var instance =
          KDDWBindingsCore.Object.s_dartInstanceByCppPtr[cppPointer.address];
      if (instance != null) return instance as ViewFactory;
    }
    return ViewFactory.fromCppPointer(cppPointer, needsAutoDelete);
  } //ViewFactory()
  ViewFactory() : super.init() {
    final voidstar_Func_void func = _dylib
        .lookup<ffi.NativeFunction<voidstar_Func_void_FFI>>(
            'c_KDDockWidgets__flutter__ViewFactory__constructor')
        .asFunction();
    thisCpp = func();
    KDDWBindingsCore.Object.s_dartInstanceByCppPtr[thisCpp.address] = this;
    registerCallbacks();
  }
  static ffi.Pointer<void> createClassicIndicatorWindow_calledFromC(
      ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? arg__1,
      ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createClassicIndicatorWindow(KDDockWidgets::Core::ClassicDropIndicatorOverlay * arg__1, KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createClassicIndicatorWindow(
        (arg__1 == null || arg__1.address == 0)
            ? null
            : KDDWBindingsCore.ClassicDropIndicatorOverlay.fromCppPointer(
                arg__1),
        parent: (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  } // createClassicIndicatorWindow_flutter(KDDockWidgets::Core::ClassicDropIndicatorOverlay * arg__1, KDDockWidgets::Core::View * parent) const

  KDDWBindingsFlutter.IndicatorWindow createClassicIndicatorWindow_flutter(
      KDDWBindingsCore.ClassicDropIndicatorOverlay? arg__1,
      {required KDDWBindingsCore.View? parent}) {
    final voidstar_Func_voidstar_voidstar_voidstar func = _dylib
        .lookup<
                ffi.NativeFunction<
                    voidstar_Func_voidstar_voidstar_voidstar_FFI>>(
            cFunctionSymbolName(595))
        .asFunction();
    ffi.Pointer<void> result = func(
        thisCpp,
        arg__1 == null ? ffi.nullptr : arg__1.thisCpp,
        parent == null ? ffi.nullptr : parent.thisCpp);
    return KDDWBindingsFlutter.IndicatorWindow.fromCppPointer(result, false);
  }

  static ffi.Pointer<void> createClassicIndicatorWindow_flutter_calledFromC(
      ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? arg__1,
      ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createClassicIndicatorWindow_flutter(KDDockWidgets::Core::ClassicDropIndicatorOverlay * arg__1, KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createClassicIndicatorWindow_flutter(
        (arg__1 == null || arg__1.address == 0)
            ? null
            : KDDWBindingsCore.ClassicDropIndicatorOverlay.fromCppPointer(
                arg__1),
        parent: (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  }

  static ffi.Pointer<void> createDockWidget_calledFromC(
      ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? uniqueName,
      int arg__2,
      int arg__3,
      int arg__4) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createDockWidget(const QString & uniqueName, QFlags<KDDockWidgets::DockWidgetOption> arg__2, QFlags<KDDockWidgets::LayoutSaverOption> arg__3, Qt::WindowFlags arg__4) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createDockWidget(
        QString.fromCppPointer(uniqueName).toDartString(),
        options: arg__2,
        layoutSaverOptions: arg__3,
        windowFlags: arg__4);
    return result.thisCpp;
  }

  static ffi.Pointer<void> createDropArea_calledFromC(ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? arg__1, ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createDropArea(KDDockWidgets::Core::DropArea * arg__1, KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createDropArea(
        (arg__1 == null || arg__1.address == 0)
            ? null
            : KDDWBindingsCore.DropArea.fromCppPointer(arg__1),
        (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  }

  static ffi.Pointer<void> createFloatingWindow_calledFromC(
      ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? arg__1,
      ffi.Pointer<void>? parent,
      int windowFlags) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createFloatingWindow(KDDockWidgets::Core::FloatingWindow * arg__1, KDDockWidgets::Core::MainWindow * parent, Qt::WindowFlags windowFlags) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createFloatingWindow(
        (arg__1 == null || arg__1.address == 0)
            ? null
            : KDDWBindingsCore.FloatingWindow.fromCppPointer(arg__1),
        parent: (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.MainWindow.fromCppPointer(parent),
        windowFlags: windowFlags);
    return result.thisCpp;
  }

  static ffi.Pointer<void> createGroup_calledFromC(ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? arg__1, ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createGroup(KDDockWidgets::Core::Group * arg__1, KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createGroup(
        (arg__1 == null || arg__1.address == 0)
            ? null
            : KDDWBindingsCore.Group.fromCppPointer(arg__1),
        parent: (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  }

  static ffi.Pointer<void> createRubberBand_calledFromC(
      ffi.Pointer<void> thisCpp, ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createRubberBand(KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createRubberBand(
        (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  }

  static ffi.Pointer<void> createSeparator_calledFromC(
      ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? arg__1,
      ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createSeparator(KDDockWidgets::Core::Separator * arg__1, KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createSeparator(
        (arg__1 == null || arg__1.address == 0)
            ? null
            : KDDWBindingsCore.Separator.fromCppPointer(arg__1),
        parent: (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  }

  static ffi.Pointer<void> createSideBar_calledFromC(ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? arg__1, ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createSideBar(KDDockWidgets::Core::SideBar * arg__1, KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createSideBar(
        (arg__1 == null || arg__1.address == 0)
            ? null
            : KDDWBindingsCore.SideBar.fromCppPointer(arg__1),
        (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  }

  static ffi.Pointer<void> createStack_calledFromC(ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? arg__1, ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createStack(KDDockWidgets::Core::Stack * arg__1, KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createStack(
        (arg__1 == null || arg__1.address == 0)
            ? null
            : KDDWBindingsCore.Stack.fromCppPointer(arg__1),
        (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  }

  static ffi.Pointer<void> createTabBar_calledFromC(ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? tabBar, ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createTabBar(KDDockWidgets::Core::TabBar * tabBar, KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createTabBar(
        (tabBar == null || tabBar.address == 0)
            ? null
            : KDDWBindingsCore.TabBar.fromCppPointer(tabBar),
        parent: (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  }

  static ffi.Pointer<void> createTitleBar_calledFromC(ffi.Pointer<void> thisCpp,
      ffi.Pointer<void>? arg__1, ffi.Pointer<void>? parent) {
    var dartInstance = KDDWBindingsCore
        .Object.s_dartInstanceByCppPtr[thisCpp.address] as ViewFactory;
    if (dartInstance == null) {
      print(
          "Dart instance not found for ViewFactory::createTitleBar(KDDockWidgets::Core::TitleBar * arg__1, KDDockWidgets::Core::View * parent) const! (${thisCpp.address})");
      throw Error();
    }
    final result = dartInstance.createTitleBar(
        (arg__1 == null || arg__1.address == 0)
            ? null
            : KDDWBindingsCore.TitleBar.fromCppPointer(arg__1),
        (parent == null || parent.address == 0)
            ? null
            : KDDWBindingsCore.View.fromCppPointer(parent));
    return result.thisCpp;
  }

  void release() {
    final void_Func_voidstar func = _dylib
        .lookup<ffi.NativeFunction<void_Func_voidstar_FFI>>(
            'c_KDDockWidgets__flutter__ViewFactory__destructor')
        .asFunction();
    func(thisCpp);
  }

  String cFunctionSymbolName(int methodId) {
    switch (methodId) {
      case 577:
        return "c_KDDockWidgets__flutter__ViewFactory__createClassicIndicatorWindow_ClassicDropIndicatorOverlay_View";
      case 595:
        return "c_KDDockWidgets__flutter__ViewFactory__createClassicIndicatorWindow_flutter_ClassicDropIndicatorOverlay_View";
      case 578:
        return "c_KDDockWidgets__flutter__ViewFactory__createDockWidget_QString_DockWidgetOptions_LayoutSaverOptions_WindowFlags";
      case 579:
        return "c_KDDockWidgets__flutter__ViewFactory__createDropArea_DropArea_View";
      case 580:
        return "c_KDDockWidgets__flutter__ViewFactory__createFloatingWindow_FloatingWindow_MainWindow_WindowFlags";
      case 581:
        return "c_KDDockWidgets__flutter__ViewFactory__createGroup_Group_View";
      case 582:
        return "c_KDDockWidgets__flutter__ViewFactory__createRubberBand_View";
      case 583:
        return "c_KDDockWidgets__flutter__ViewFactory__createSeparator_Separator_View";
      case 584:
        return "c_KDDockWidgets__flutter__ViewFactory__createSideBar_SideBar_View";
      case 585:
        return "c_KDDockWidgets__flutter__ViewFactory__createStack_Stack_View";
      case 586:
        return "c_KDDockWidgets__flutter__ViewFactory__createTabBar_TabBar_View";
      case 587:
        return "c_KDDockWidgets__flutter__ViewFactory__createTitleBar_TitleBar_View";
    }
    return super.cFunctionSymbolName(methodId);
  }

  static String methodNameFromId(int methodId) {
    switch (methodId) {
      case 577:
        return "createClassicIndicatorWindow";
      case 595:
        return "createClassicIndicatorWindow_flutter";
      case 578:
        return "createDockWidget";
      case 579:
        return "createDropArea";
      case 580:
        return "createFloatingWindow";
      case 581:
        return "createGroup";
      case 582:
        return "createRubberBand";
      case 583:
        return "createSeparator";
      case 584:
        return "createSideBar";
      case 585:
        return "createStack";
      case 586:
        return "createTabBar";
      case 587:
        return "createTitleBar";
    }
    throw Error();
  }

  void registerCallbacks() {
    assert(thisCpp != null);
    final RegisterMethodIsReimplementedCallback registerCallback = _dylib
        .lookup<ffi.NativeFunction<RegisterMethodIsReimplementedCallback_FFI>>(
            'c_KDDockWidgets__flutter__ViewFactory__registerVirtualMethodCallback')
        .asFunction();
    final callback577 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter
                .ViewFactory.createClassicIndicatorWindow_calledFromC);
    registerCallback(thisCpp, callback577, 577);
    final callback595 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter
                .ViewFactory.createClassicIndicatorWindow_flutter_calledFromC);
    registerCallback(thisCpp, callback595, 595);
    final callback578 = ffi.Pointer.fromFunction<
            voidstar_Func_voidstar_voidstar_ffi_Int32_ffi_Int32_ffi_Int32_FFI>(
        KDDWBindingsFlutter.ViewFactory.createDockWidget_calledFromC);
    registerCallback(thisCpp, callback578, 578);
    final callback579 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter.ViewFactory.createDropArea_calledFromC);
    registerCallback(thisCpp, callback579, 579);
    final callback580 = ffi.Pointer.fromFunction<
            voidstar_Func_voidstar_voidstar_voidstar_ffi_Int32_FFI>(
        KDDWBindingsFlutter.ViewFactory.createFloatingWindow_calledFromC);
    registerCallback(thisCpp, callback580, 580);
    final callback581 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter.ViewFactory.createGroup_calledFromC);
    registerCallback(thisCpp, callback581, 581);
    final callback582 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter.ViewFactory.createRubberBand_calledFromC);
    registerCallback(thisCpp, callback582, 582);
    final callback583 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter.ViewFactory.createSeparator_calledFromC);
    registerCallback(thisCpp, callback583, 583);
    final callback584 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter.ViewFactory.createSideBar_calledFromC);
    registerCallback(thisCpp, callback584, 584);
    final callback585 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter.ViewFactory.createStack_calledFromC);
    registerCallback(thisCpp, callback585, 585);
    final callback586 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter.ViewFactory.createTabBar_calledFromC);
    registerCallback(thisCpp, callback586, 586);
    final callback587 =
        ffi.Pointer.fromFunction<voidstar_Func_voidstar_voidstar_voidstar_FFI>(
            KDDWBindingsFlutter.ViewFactory.createTitleBar_calledFromC);
    registerCallback(thisCpp, callback587, 587);
  }
}
