//

#ifndef _SECP256K1_SCRATCH_
#define _SECP256K1_SCRATCH_

#define SECP256K1_SCRATCH_MAX_FRAMES 5

typedef struct secp256k1_scratch_space_struct {
  void *data[SECP256K1_SCRATCH_MAX_FRAMES];
  size_t offset[SECP256K1_SCRATCH_MAX_FRAMES];
  size_t frame_size[SECP256K1_SCRATCH_MAX_FRAMES];
  size_t frame;
  size_t max_size;
  const secp256k1_callback* error_callback;
} secp256k1_scratch;

static secp256k1_scratch* secp256k1_scratch_create(const secp256k1_callback* error_callback, size_t max_size);

static void secp256k1_scratch_destroy(secp256k1_scratch* scratch);

static int secp256k1_scratch_allocate_frame(secp256k1_scratch* scratch, size_t n, size_t objects);

static size_t secp256k1_scratch_max_allocation(const secp256k1_scratch* scartch, size_t n_objects);

static void *secp256k1_scratch_alloc(secp256k1_scratch* scratch, size_t n);

#endif

