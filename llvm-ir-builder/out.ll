; ModuleID = 'ir-builder'
source_filename = "ir-builder"
target triple = "x86_64-unknown-linux-gnu"

%struct.point = type { i32, i32 }

@global_a = dso_local global i32 1
@__constant.main.init_array = private constant [4 x i32] [i32 1, i32 2, i32 3, i32 4]
@__constant.main.point = private constant %struct.point { i32 1, i32 2 }
@.string = private unnamed_addr constant [6 x i8] c"hello\00", align 1

define dso_local i32 @main() {
entry:
  %0 = load i32, ptr @global_a, align 4
  ret i32 %0
}
