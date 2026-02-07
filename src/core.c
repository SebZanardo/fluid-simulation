#include "core.h"

// OS /////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include <windows.h>

static u32 OS_PageSize(void) {
    SYSTEM_INFO sysinfo = { 0 };
    GetSystemInfo(&sysinfo);
    return sysinfo.dwPageSize;
}

static void* OS_MemoryReserve(u64 size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

static b32 OS_MemoryCommit(void* ptr, u64 size) {
    return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL;
}

/*
static b32 OS_MemoryDecommit(void* ptr, u64 size) {
    return VirtualFree(ptr, size, MEM_DECOMMIT);
}
*/

static b32 OS_MemoryRelease(void* ptr, u64 size) {
    return VirtualFree(ptr, size, MEM_RELEASE);
}

#else
#include <unistd.h>
#include <sys/mman.h>

static u32 OS_PageSize(void) {
    return (u32)sysconf(_SC_PAGESIZE);
}

static void* OS_MemoryReserve(u64 size) {
    void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (ptr == MAP_FAILED) ? NULL : ptr;
}

static b32 OS_MemoryCommit(void* ptr, u64 size) {
    return mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0;
}

/*
static b32 OS_MemoryDecommit(void* ptr, u64 size) {
    madvise(ptr, size, MADV_DONTNEED);
    return mprotect(ptr, size, PROT_NONE) == 0;
}
*/

static b32 OS_MemoryRelease(void* ptr, u64 size) {
    return munmap(ptr, size) == 0;
}

#endif

// ARENA //////////////////////////////////////////////////////////////////////
static const u64 ARENA_BASE_POS = sizeof(Arena);
static const u64 ARENA_ALIGN = sizeof(void*);

Arena* ArenaCreate(u64 reserve_size, u64 commit_size) {
    u32 pagesize = OS_PageSize();

    reserve_size = AlignUpPow2(reserve_size, pagesize);
    commit_size = AlignUpPow2(commit_size, pagesize);

    Arena* arena = OS_MemoryReserve(reserve_size);

    assert(OS_MemoryCommit(arena, commit_size) /*Unable to allocate arena*/);

    arena->reserve_size = reserve_size;
    arena->commit_size = commit_size;
    arena->base = ARENA_BASE_POS;
    arena->base_next = commit_size;

    return arena;
}

void ArenaDestroy(Arena* arena) {
    OS_MemoryRelease(arena, arena->reserve_size);
}

void* ArenaPush(Arena* arena, u64 size, b32 zero) {
    u64 pos_aligned = AlignUpPow2(arena->base, ARENA_ALIGN);
    u64 new_pos = pos_aligned + size;

    if (new_pos > arena->reserve_size) { return NULL; }

    if (new_pos > arena->base_next) {
        u64 new_commit_pos = new_pos;
        new_commit_pos += arena->commit_size - 1;
        new_commit_pos -= new_commit_pos % arena->commit_size;
        new_commit_pos = Min(new_commit_pos, arena->reserve_size);

        u8* mem = (u8*)arena + arena->base_next;
        u64 commit_size = new_commit_pos - arena->base_next;

        if (!OS_MemoryCommit(mem, commit_size)) { return NULL; }

        arena->base_next = new_commit_pos;
    }

    arena->base = new_pos;

    u8* out = (u8*)arena + pos_aligned;
    if (zero) { memset(out, 0, size); }
    return out;
}

void ArenaPop(Arena* arena, u64 size) {
    size = Min(size, arena->base - ARENA_BASE_POS);
    arena->base -= size;
}

void ArenaPopTo(Arena* arena, u64 pos) {
    u64 size = pos < arena->base ? arena->base - pos : 0;
    ArenaPop(arena, size);
}

void ArenaClear(Arena* arena) {
    ArenaPopTo(arena, ARENA_BASE_POS);
}

// RANDOM /////////////////////////////////////////////////////////////////////
void RandomSetSeed(RNG* rng, u64 initstate, u64 initseq) {
    rng->state = 0;
    rng->inc = (initseq << 1) | 1;
    Random_u32(rng);
    rng->state += initstate;
    Random_u32(rng);
};

u32 Random_u32(RNG* rng) {
    u64 oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    u32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    u32 rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
};

f32 Random_f32(RNG* rng) {
    return (f32)Random_u32(rng) / (f32)MAX_U32;
};

static f32 RandomNorm(RNG* rng) {
    f32 u1 = 0.0f;
    do { u1 = Random_f32(rng); } while (u1 == 0.0f);
    f32 u2 = Random_f32(rng);

    f32 magnitude = sqrt_f32(-2.0f * log_f32(u1));
    f32 value = magnitude * cos_f32(2.0 * PI * u2);
    // NOTE: Throwing away free second random value
    // f32 value2 = magnitude * sin_f32(2.0 * PI * u2);

    return value;
}

f32 RandomNormBetween(RNG* rng, f32 min, f32 max) {
    f32 center = (min + max) / 2.0f;
    f32 stddev = (max - min) / 6.0f;

    f32 value;
    do {
        value = center + RandomNorm(rng) * stddev;
    } while (value < min || value > max);

    return value;
}

Vector2 RandomCircle(RNG* rng, Vector2 position, f32 radius) {
    f32 r = radius * sqrt_f32(Random_f32(rng));
    f32 theta = Random_f32(rng) * 2 * PI;
    return (Vector2) {
        position.x + r * cos_f32(theta),
        position.y + r * sin_f32(theta)
    };
};
