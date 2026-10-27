; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [6 x i8] c"%d - \00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 0, ptr %0, align 4
  store i32 0, ptr %1, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.step, %entry
  %2 = load i32, ptr %1, align 4
  %lt = icmp slt i32 %2, 20
  br i1 %lt, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %3 = load i32, ptr %0, align 4
  %add = add i32 100, %3
  store i32 %add, ptr %0, align 4
  %4 = load i32, ptr %0, align 4
  %5 = call i32 (ptr, ...) @printf(ptr @.str, i32 %4)
  br label %for.step

for.step:                                         ; preds = %for.body
  %6 = load i32, ptr %1, align 4
  %add1 = add i32 %6, 1
  store i32 %add1, ptr %1, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret i32 0
}
