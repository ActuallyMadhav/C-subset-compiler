; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [7 x i8] c"break\0A\00"
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str.2 = private unnamed_addr constant [10 x i8] c"continue\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 0, ptr %0, align 4
  br label %while.cond

while.cond:                                       ; preds = %if.end3, %if.then, %entry
  %2 = load i32, ptr %1, align 4
  %lt = icmp slt i32 %2, 20
  br i1 %lt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %3 = load i32, ptr %1, align 4
  %inc = add i32 %3, 1
  store i32 %inc, ptr %1, align 4
  %4 = load i32, ptr %1, align 4
  %eq = icmp eq i32 %4, 5
  br i1 %eq, label %if.then, label %if.end

while.end:                                        ; preds = %if.then2, %while.cond
  ret i32 0

if.then:                                          ; preds = %while.body
  %5 = call i32 (ptr, ...) @printf(ptr @.str.2)
  br label %while.cond

if.end:                                           ; preds = %while.body
  %6 = load i32, ptr %0, align 4
  %add = add i32 100, %6
  store i32 %add, ptr %0, align 4
  %7 = load i32, ptr %0, align 4
  %8 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %7)
  %9 = load i32, ptr %1, align 4
  %eq1 = icmp eq i32 %9, 15
  br i1 %eq1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  %10 = call i32 (ptr, ...) @printf(ptr @.str)
  br label %while.end

if.end3:                                          ; preds = %if.end
  br label %while.cond
}
