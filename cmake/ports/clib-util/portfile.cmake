# header-only library
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO powerof3/CLibUtil
    REF 3defe39f14a4e5fd24e9544f2a0c1b6d669d9b72
    SHA512 3ee55db0c955021d1f809afb3d2d886757c3ce70b44d58e2753fd1784c38a886fb533cda5dbe0b58fa95d30334738cfd948175d18b2b395340e6b4bcfeeb4cfc
    HEAD_REF master
)

# Install codes
set(CLIBUTIL_SOURCE	${SOURCE_PATH}/include/ClibUtil)
file(INSTALL ${CLIBUTIL_SOURCE} DESTINATION ${CURRENT_PACKAGES_DIR}/include)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
