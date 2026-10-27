; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [8 x i8] c"y != 1\0A\00"
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 1, ptr %1, align 4
  store i32 5, ptr %0, align 4
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %2 = load i32, ptr %1, align 4
  %tobool = icmp ne i32 %2, 0
  br i1 %tobool, label %and.rhs, label %and.end

and.rhs:                                          ; preds = %while.cond
  %3 = load i32, ptr %0, align 4
  %tobool1 = icmp ne i32 %3, 0
  br label %and.end

and.end:                                          ; preds = %and.rhs, %while.cond
  %4 = phi i1 [ false, %while.cond ], [ %tobool1, %and.rhs ]
  %5 = zext i1 %4 to i32
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %while.body, label %while.end

while.body:                                       ; preds = %and.end
  %7 = load i32, ptr %0, align 4
  %8 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %7)
  %9 = load i32, ptr %0, align 4
  %dec = sub i32 %9, 1
  store i32 %dec, ptr %0, align 4
  %10 = load i32, ptr %0, align 4
  %eq = icmp eq i32 %10, 1
  br i1 %eq, label %if.then, label %if.else

while.end:                                        ; preds = %and.end
  ret i32 0

if.then:                                          ; preds = %while.body
  store i32 0, ptr %1, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %if.else
  br label %while.cond

if.else:                                          ; preds = %while.body
  %11 = call i32 (ptr, ...) @printf(ptr @.str)
  br label %if.end
}
