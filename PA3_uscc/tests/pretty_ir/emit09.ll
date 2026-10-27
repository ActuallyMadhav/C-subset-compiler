; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %2, align 4
  store i32 0, ptr %1, align 4
  store i32 1, ptr %0, align 4
  %3 = load i32, ptr %2, align 4
  %tobool = icmp ne i32 %3, 0
  br i1 %tobool, label %lor.end2, label %lor.rhs1

lor.rhs:                                          ; preds = %lor.end2
  %4 = load i32, ptr %0, align 4
  %tobool5 = icmp ne i32 %4, 0
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %lor.end2
  %5 = phi i1 [ true, %lor.end2 ], [ %tobool5, %lor.rhs ]
  %6 = zext i1 %5 to i32
  %7 = call i32 (ptr, ...) @printf(ptr @.str, i32 %6)
  ret i32 0

lor.rhs1:                                         ; preds = %entry
  %8 = load i32, ptr %1, align 4
  %tobool3 = icmp ne i32 %8, 0
  br label %lor.end2

lor.end2:                                         ; preds = %lor.rhs1, %entry
  %9 = phi i1 [ true, %entry ], [ %tobool3, %lor.rhs1 ]
  %10 = zext i1 %9 to i32
  %tobool4 = icmp ne i32 %10, 0
  br i1 %tobool4, label %lor.end, label %lor.rhs
}
