; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [27 x i8] c"Superuser access enabled.\0A\00"
@.str.1 = private unnamed_addr constant [23 x i8] c"Admin access enabled.\0A\00"
@.str.2 = private unnamed_addr constant [22 x i8] c"User access enabled.\0A\00"
@.str.3 = private unnamed_addr constant [23 x i8] c"Guest access enabled.\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  store i32 2, ptr %0, align 4
  %1 = load i32, ptr %0, align 4
  switch i32 %1, label %switch.end [
    i32 1, label %switch.case
    i32 2, label %switch.case1
    i32 3, label %switch.case2
    i32 4, label %switch.case3
  ]

switch.end:                                       ; preds = %switch.case3, %switch.case2, %entry
  ret i32 0

switch.case:                                      ; preds = %entry
  %2 = call i32 (ptr, ...) @printf(ptr @.str.3)
  br label %switch.case1

switch.case1:                                     ; preds = %switch.case, %entry
  %3 = call i32 (ptr, ...) @printf(ptr @.str.2)
  br label %switch.case2

switch.case2:                                     ; preds = %switch.case1, %entry
  %4 = call i32 (ptr, ...) @printf(ptr @.str.1)
  br label %switch.end

switch.case3:                                     ; preds = %entry
  %5 = call i32 (ptr, ...) @printf(ptr @.str)
  br label %switch.end
}
