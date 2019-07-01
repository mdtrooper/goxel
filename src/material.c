/* Goxel 3D voxels editor
 *
 * copyright (c) 2019 Guillaume Chereau <guillaume@noctua-software.com>
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


#include "material.h"

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>


material_t *material_new(const char *name)
{
    material_t *m = calloc(1, sizeof(*m));
    *m = MATERIAL_DEFAULT;
    if (name) snprintf(m->name, sizeof(m->name), "%s", name);
    return m;
}

void material_delete(material_t *m)
{
    free(m);
}

material_t *material_copy(const material_t *other)
{
    material_t *m = malloc(sizeof(*m));
    *m = *other;
    m->next = m->prev = NULL;
    return m;
}

uint32_t material_get_hash(const material_t *m)
{
    uint32_t ret = 0;
    ret = crc32(ret, (void*)&m->metallic, sizeof(m->metallic));
    ret = crc32(ret, (void*)&m->roughness, sizeof(m->roughness));
    ret = crc32(ret, (void*)&m->base_color, sizeof(m->base_color));
    return ret;
}
