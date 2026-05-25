# CMakeList.txt: Üst düzey CMake proje dosyası, genel yapılandırma gerçekleştirin
# ve alt projeleri buraya ekleyin.
#
cmake_minimum_required (VERSION 3.8)

# Destekleniyorsa MSVC derleyicileri için Çalışırken Yeniden Yükleme aracını etkinleştirin.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CMakeProject1")

# Alt projeleri ekleyin.
add_subdirectory ("CMakeProject1")
