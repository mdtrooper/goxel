/* Goxel 3D voxels editor
 *
 * copyright (c) 2017 Guillaume Chereau <guillaume@noctua-software.com>
 *
 * Goxel is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.

 * Goxel is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.

 * You should have received a copy of the GNU General Public License along with
 * goxel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "goxel.h"
#include "file_format.h"
#include <errno.h>

static int export_as_txt(const image_t *image, const char *path)
{
    FILE *out;
    const mesh_t *mesh = goxel_get_layers_mesh(image);
    int p[3];
    uint8_t v[4];
    mesh_iterator_t iter;

    out = fopen(path, "w");
    if (!out) {
        LOG_E("Cannot save to %s: %s", path, strerror(errno));
        return -1;
    }
    fprintf(out, "# Goxel " GOXEL_VERSION_STR "\n");
    fprintf(out, "# One line per voxel\n");
    fprintf(out, "# X Y Z RRGGBB\n");

    iter = mesh_get_iterator(mesh, MESH_ITER_VOXELS);
    while (mesh_iter(&iter, p)) {
        mesh_get_at(mesh, &iter, p, v);
        if (v[3] < 127) continue;
        fprintf(out, "%d %d %d %02x%02x%02x\n",
                p[0], p[1], p[2], v[0], v[1], v[2]);
    }
    fclose(out);
    return 0;
}

FILE_FORMAT_REGISTER(txt,
    .name = "text",
    .ext = "text\0*.txt\0",
    .export_func = export_as_txt,
)
