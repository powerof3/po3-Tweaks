# header-only library
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO powerof3/CLibUtil
    REF 4c942f62d83cfbdb9bb585a708cbbfbf5cb9c850
    SHA512 8130bf589476202dcae05704145b0cbca3587e2458f1f79ee497f45fdc7dddcbc6a0e7311dd0566eb46633da963203bf0d9864debdd2406c317bbb18b484e184
    HEAD_REF master
)

# Install codes
set(CLIBUTIL_SOURCE	${SOURCE_PATH}/include/ClibUtil)
file(INSTALL ${CLIBUTIL_SOURCE} DESTINATION ${CURRENT_PACKAGES_DIR}/include)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
