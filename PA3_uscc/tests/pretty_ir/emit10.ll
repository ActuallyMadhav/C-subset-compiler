; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%s\0A\00"
@.str.1 = private unnamed_addr constant [36 x i8] c"thequickbrownfoxjumpsoverthelazydog\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %str = alloca [36 x i8], align 8
  %0 = getelementptr inbounds [36 x i8], ptr %str, i32 0, i32 0
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 @.str.1, i64 36, i1 false)
  %1 = call i32 (ptr, ...) @printf(ptr @.str, ptr %0)
  %2 = getelementptr inbounds i8, ptr %0, i32 8
  %3 = call i32 (ptr, ...) @printf(ptr @.str, ptr %2)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
