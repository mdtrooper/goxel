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

#include "goxel.h"

#include <zlib.h> // For crc32

layer_t *layer_new(const char *name)
{
    layer_t *layer;
    layer = calloc(1, sizeof(*layer));
    if (name) strncpy(layer->name, name, sizeof(layer->name) - 1);
    layer->mesh = mesh_new();
    mat4_set_identity(layer->mat);
    return layer;
}

void layer_delete(layer_t *layer)
{
    mesh_delete(layer->mesh);
    texture_delete(layer->image);
    free(layer);
}

uint32_t layer_get_key(const layer_t *layer)
{
    uint32_t key;
    key = mesh_get_key(layer->mesh);
    key = crc32(key, (void*)&layer->visible, sizeof(layer->visible));
    key = crc32(key, (void*)&layer->name, sizeof(layer->name));
    key = crc32(key, (void*)&layer->box, sizeof(layer->box));
    key = crc32(key, (void*)&layer->mat, sizeof(layer->mat));
    key = crc32(key, (void*)&layer->shape, sizeof(layer->shape));
    key = crc32(key, (void*)&layer->color, sizeof(layer->color));
    key = crc32(key, (void*)&layer->material, sizeof(layer->material));
    return key;
}

layer_t *layer_copy(layer_t *other)
{
    layer_t *layer;
    layer = calloc(1, sizeof(*layer));
    memcpy(layer->name, other->name, sizeof(layer->name));
    layer->visible = other->visible;
    layer->mesh = mesh_copy(other->mesh);
    layer->image = texture_copy(other->image);
    layer->material = other->material;
    mat4_copy(other->box, layer->box);
    mat4_copy(other->mat, layer->mat);
    layer->id = other->id;
    layer->base_id = other->base_id;
    layer->base_mesh_key = other->base_mesh_key;
    layer->shape = other->shape;
    layer->shape_key = other->shape_key;
    memcpy(layer->color, other->color, sizeof(layer->color));
    return layer;
}
