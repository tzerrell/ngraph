//*****************************************************************************
// Copyright 2017-2019 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#include <sstream>

#include "ngraph/file_util.hpp"
#include "ngraph/runtime/backend.hpp"
#include "ngraph/runtime/backend_manager.hpp"
#include "ngraph/runtime/dynamic/dynamic_backend.hpp"
#include "ngraph/util.hpp"

using namespace std;
using namespace ngraph;

runtime::Backend::~Backend()
{
}

std::shared_ptr<ngraph::Node> runtime::Backend::get_backend_op(const std::string& op_name, ...)
{
    std::shared_ptr<ngraph::Node> dummy_node(nullptr);
    return dummy_node;
}

std::shared_ptr<runtime::Backend> runtime::Backend::create(const string& type,
                                                           bool must_support_dynamic)
{
    auto inner_backend = BackendManager::create_backend(type);

    if (!must_support_dynamic || inner_backend->supports_dynamic_tensors())
    {
        return inner_backend;
    }
    else
    {
        return make_shared<runtime::dynamic::DynamicBackend>(inner_backend);
    }
}

vector<string> runtime::Backend::get_registered_devices()
{
    return BackendManager::get_registered_backends();
}

std::shared_ptr<ngraph::runtime::Tensor>
    runtime::Backend::create_dynamic_tensor(const ngraph::element::Type& element_type,
                                            const PartialShape& shape)
{
    throw std::invalid_argument("This backend does not support dynamic tensors");
}

std::shared_ptr<runtime::Executable>
    runtime::Backend::compile(std::shared_ptr<Function> func,
                              ngraph::pass::PassConfig& pass_config,
                              bool enable_performance_data)
{
    return compile(func, enable_performance_data);
}

bool runtime::Backend::is_supported(const Node& node) const
{
    // The default behavior is that a backend does not support any ops. If this is not the case
    // then override this method and enhance.
    return false;
}

bool runtime::Backend::is_supported_property(const Property prop) const
{
    return false;
}

void runtime::Backend::remove_compiled_function(std::shared_ptr<Executable> exec)
{
}

std::shared_ptr<runtime::Executable> runtime::Backend::load(istream& input_stream)
{
    throw runtime_error("load opertion unimplemented.");
}

bool runtime::Backend::set_config(const map<string, string>& config, string& error)
{
    error = "set_config not supported";
    return false;
}
