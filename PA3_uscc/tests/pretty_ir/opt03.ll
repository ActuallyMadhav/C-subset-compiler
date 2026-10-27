; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, ptr %3, align 4
  store i32 1, ptr %2, align 4
  store i32 2, ptr %1, align 4
  store i32 0, ptr %0, align 4
  %4 = load i32, ptr %3, align 4
  %tobool = icmp ne i32 %4, 0
  br i1 %tobool, label %and.rhs, label %and.end

lor.rhs:                                          ; preds = %and.end
  %5 = load i32, ptr %2, align 4
  %tobool3 = icmp ne i32 %5, 0
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %and.end
  %6 = phi i1 [ true, %and.end ], [ %tobool3, %lor.rhs ]
  %7 = zext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %if.then, label %if.else

and.rhs:                                          ; preds = %entry
  %9 = load i32, ptr %1, align 4
  %tobool1 = icmp ne i32 %9, 0
  br label %and.end

and.end:                                          ; preds = %and.rhs, %entry
  %10 = phi i1 [ false, %entry ], [ %tobool1, %and.rhs ]
  %11 = zext i1 %10 to i32
  %tobool2 = icmp ne i32 %11, 0
  br i1 %tobool2, label %lor.end, label %lor.rhs

if.then:                                          ; preds = %lor.end
  store i32 20, ptr %0, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %if.else
  %12 = load i32, ptr %0, align 4
  %13 = call i32 (ptr, ...) @printf(ptr @.str, i32 %12)
  ret i32 0

if.else:                                          ; preds = %lor.end
  store i32 10, ptr %0, align 4
  br label %if.end
}
