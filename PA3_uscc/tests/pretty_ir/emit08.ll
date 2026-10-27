; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 0, ptr %0, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %2 = load i32, ptr %0, align 4
  %lt = icmp slt i32 %2, 2000000000
  br i1 %lt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  store i32 0, ptr %1, align 4
  %3 = load i32, ptr %0, align 4
  %inc = add i32 %3, 1
  store i32 %inc, ptr %0, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %4 = load i32, ptr %0, align 4
  %5 = call i32 (ptr, ...) @printf(ptr @.str, i32 %4)
  ret i32 0
}
