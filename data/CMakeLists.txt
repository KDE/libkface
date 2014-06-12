# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2010-06-16
# @brief  LibKFace library interface for KDE
#
# @author Copyright (C) 2010-2012 by Gilles Caulier
#         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# ============================================================

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
INSTALL( FILES ${files}
         DESTINATION ${DATA_INSTALL_DIR}/libkface/haarcascades )

ADD_CUSTOM_COMMAND(OUTPUT face-funnel.data
                   COMMAND ${CMAKE_COMMAND} -E tar xz ${CMAKE_CURRENT_SOURCE_DIR}/face-funnel.data.tgz)
ADD_CUSTOM_TARGET(extract_funnel_data ALL DEPENDS face-funnel.data)
INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/face-funnel.data
         DESTINATION ${DATA_INSTALL_DIR}/libkface/alignment-congealing)

#ADD_CUSTOM_COMMAND(OUTPUT flandmark_model.dat
 #                  COMMAND ${CMAKE_COMMAND} -E tar xz ${CMAKE_CURRENT_SOURCE_DIR}/flandmark_model.dat.tgz)
#ADD_CUSTOM_TARGET(extract_flandmark_data ALL DEPENDS flandmark_model.dat)
#INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/flandmark_model.dat
 #        DESTINATION ${DATA_INSTALL_DIR}/libkface/alignment-flandmark)
