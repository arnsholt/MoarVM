#include "moar.h"

/* Tries to generate a specialization of the bytecode, for the given callsite
 * and argument tuple. */
MVMSpeshCandidate * MVM_spesh_candidate_generate(MVMThreadContext *tc,
        MVMStaticFrame *static_frame, MVMCallsite *callsite, MVMRegister *args) {
    MVMSpeshCandidate *result;
    MVMSpeshCode *sc;

    /* Generate the specialization. */
    MVMSpeshGraph *sg = MVM_spesh_graph_create(tc, static_frame);
    /* XXX Apply the specialization here, of course. */
    sc = MVM_spesh_codegen(tc, sg);
    MVM_spesh_graph_destroy(tc, sg);

    /* Now try to add it. Note there's a slim chance another thread beat us
     * to doing so. Also other threads can read the specializations without
     * lock, so make absolutely sure we increment the count of them after we
     * add the new one. */
    result    = NULL;
    uv_mutex_lock(&tc->instance->mutex_spesh_install);
    if (static_frame->body.num_spesh_candidates < MVM_SPESH_LIMIT) {
        MVMint32 num_spesh = static_frame->body.num_spesh_candidates;
        MVMint32 i;
        for (i = 0; i < num_spesh; i++)
            if (static_frame->body.spesh_candidates[i].cs == callsite) {
                /* Beaten! */
                result = &static_frame->body.spesh_candidates[i];
                break;
            }
        if (!result) {
            if (!static_frame->body.spesh_candidates)
                static_frame->body.spesh_candidates = malloc(
                    MVM_SPESH_LIMIT * sizeof(MVMSpeshCandidate));
            result                = &static_frame->body.spesh_candidates[num_spesh];
            result->cs            = callsite;
            result->bytecode      = sc->bytecode;
            result->bytecode_size = sc->bytecode_size;
            result->handlers      = sc->handlers;
            MVM_barrier();
            static_frame->body.num_spesh_candidates++;
        }
    }
    if (!result) {
        free(sc->bytecode);
        free(sc->handlers);
    }
    uv_mutex_unlock(&tc->instance->mutex_spesh_install);

    free(sc);
    return result;
}