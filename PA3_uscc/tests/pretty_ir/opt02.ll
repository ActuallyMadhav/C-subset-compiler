; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 20, ptr %1, align 4
  store i32 55, ptr %0, align 4
  %2 = load i32, ptr %1, align 4
  %ne = icmp ne i32 %2, 20
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  store i32 90, ptr %0, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %if.else
  %3 = load i32, ptr %0, align 4
  %4 = call i32 (ptr, ...) @printf(ptr @.str, i32 %3)
  ret i32 0

if.else:                                          ; preds = %entry
  store i32 100, ptr %0, align 4
  br label %if.end
}
