; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [11 x i8] c"Wednesday\0A\00"
@.str.1 = private unnamed_addr constant [9 x i8] c"Tuesday\0A\00"
@.str.2 = private unnamed_addr constant [8 x i8] c"Monday\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 3, ptr %0, align 4
  %2 = load i32, ptr %0, align 4
  switch i32 %2, label %switch.end [
    i32 1, label %switch.case
    i32 2, label %switch.case1
    i32 3, label %switch.case2
  ]

switch.end:                                       ; preds = %switch.case2, %switch.case1, %switch.case, %entry
  ret i32 0

switch.case:                                      ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @.str.2)
  br label %switch.end

switch.case1:                                     ; preds = %entry
  %4 = call i32 (ptr, ...) @printf(ptr @.str.1)
  br label %switch.end

switch.case2:                                     ; preds = %entry
  %5 = call i32 (ptr, ...) @printf(ptr @.str)
  br label %switch.end
}
