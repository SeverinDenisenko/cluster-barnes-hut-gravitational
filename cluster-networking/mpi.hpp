#pragma once

// https://github.com/open-mpi/ompi/issues/5157
// I don't know this is more funny or sad.
// Why do we have things like that?
// :'(

#define OMPI_SKIP_MPICXX 1
#include <mpi.h>
