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

#include "gtest/gtest.h"

#include "ngraph/file_util.hpp"
#include "ngraph/ngraph.hpp"
#include "ngraph/serializer.hpp"
#include "util/test_tools.hpp"

#include <memory>
using namespace std;
using namespace ngraph;

TEST(build_graph, build_simple)
{
    // Function with 4 parameters
    auto arg0 = make_shared<op::Parameter>(element::f32, Shape{7, 3});
    auto arg1 = make_shared<op::Parameter>(element::f32, Shape{3});
    auto arg2 = make_shared<op::Parameter>(element::f32, Shape{32, 7});
    auto arg3 = make_shared<op::Parameter>(element::f32, Shape{32, 7});
    auto broadcast_1 = make_shared<op::Broadcast>(arg3, Shape{10, 32, 7}, AxisSet{0});
    auto b1 = make_shared<op::Broadcast>(arg3, Shape{10, 32, 7}, AxisSet{0});
    auto dot = make_shared<op::Dot>(arg2, arg0);
    ASSERT_EQ(dot->get_arguments()[0], arg2);
    ASSERT_EQ(dot->get_arguments()[1], arg0);

    auto cluster_0 = make_shared<Function>(dot, ParameterVector{arg0, arg1, arg2, arg3});

    ASSERT_EQ(cluster_0->get_output_op(0)->get_argument(0), dot);
}

// Check node comparisons
TEST(build_graph, node_comparison)
{
    auto arg0 = make_shared<op::Parameter>(element::f32, Shape{32, 3});
    auto arg1 = make_shared<op::Parameter>(element::f32, Shape{3});
    auto arg2 = make_shared<op::Parameter>(element::f32, Shape{32});

    auto dot = make_shared<op::Dot>(arg0, arg1);
    auto add = make_shared<op::Add>(dot, arg2);

    auto parg = make_shared<op::Parameter>(element::f32, Shape{});
    auto pattern_dot = make_shared<op::Dot>(parg, parg);
}

TEST(build_graph, literal)
{
    // float scalar from a float
    // auto float0 = FloatConstant::make(3.0);
    vector<float> float_t{3.0};
    auto float0 = make_shared<op::Constant>(element::f32, Shape{}, float_t);
    ASSERT_EQ(float0->get_vector<float>(), std::vector<float>{3.0});
    ASSERT_EQ(float0->get_element_type(), element::f32);
    ASSERT_EQ(float0->get_shape(), Shape{});
    auto d = make_shared<op::Dot>(float0, float0);
    ASSERT_EQ(d->get_arguments().at(0), float0);
    ASSERT_EQ(d->get_arguments().at(1), float0);

    vector<int32_t> int32{3};
    auto int32_0 = make_shared<op::Constant>(element::i32, Shape{}, int32);
    ASSERT_EQ(int32_0->get_vector<int32_t>(), std::vector<int>{3});
    ASSERT_EQ(int32_0->get_element_type(), element::i32);
    ASSERT_EQ(int32_0->get_shape(), Shape{});
}

TEST(build_graph, tensor)
{
    // float scalar from a float
    // auto float0 = FloatConstant::make(3.0);
    Shape shape{2, 3};
    vector<float> float_t(shape_size(shape), 0);
    auto float0 = make_shared<op::Constant>(element::f32, shape, float_t);
    ASSERT_EQ(float0->get_element_type(), element::f32);
    ASSERT_EQ(float0->get_shape(), shape);
    auto d = make_shared<op::Add>(float0, float0);
    ASSERT_EQ(d->get_arguments().at(0), float0);
    ASSERT_EQ(d->get_arguments().at(1), float0);

    Shape ishape{3, 5};
    vector<int32_t> idata(shape_size(ishape), 0);
    auto int32_0 = make_shared<op::Constant>(element::i32, ishape, idata);
    ASSERT_EQ(int32_0->get_element_type(), element::i32);
    ASSERT_EQ(int32_0->get_shape(), ishape);
}

// Check functions with undeclared parameters
TEST(build_graph, function_undeclared_parameters)
{
    // Function with 4 parameters
    auto arg0 = make_shared<op::Parameter>(element::f32, Shape{7, 3});
    auto arg1 = make_shared<op::Parameter>(element::f32, Shape{3});
    auto arg2 = make_shared<op::Parameter>(element::f32, Shape{32, 7});
    auto arg3 = make_shared<op::Parameter>(element::f32, Shape{32, 7});
    auto broadcast_1 = make_shared<op::Broadcast>(arg3, Shape{10, 32, 7}, AxisSet{0});
    auto b1 = make_shared<op::Broadcast>(arg3, Shape{10, 32, 7}, AxisSet{0});
    auto dot = make_shared<op::Dot>(arg2, arg0);
    ASSERT_EQ(dot->get_arguments()[0], arg2);
    ASSERT_EQ(dot->get_arguments()[1], arg0);
    try
    {
        auto f = make_shared<Function>(dot, ParameterVector{arg0, arg1, arg3});
        f->get_ops();
        // Should have thrown, so fail if it didn't
        FAIL() << "Undeclared parameter not detected.";
    }
    catch (const ngraph_error& error)
    {
        EXPECT_EQ(error.what(), std::string("Function references undeclared parameter"));
    }
    catch (...)
    {
        FAIL() << "Function construction failed for unexpected reason";
    }
}

// Check no-arg construction
TEST(build_graph, no_arg_construction)
{
    // The ops
    // Parameters aren't converted yet
    auto arg0 = make_shared<op::Parameter>(element::f32, Shape{7});
    auto arg1 = make_shared<op::Parameter>(element::f32, Shape{7});
    auto arg2 = make_shared<op::Parameter>(element::f32, Shape{7});
    auto arg3 = make_shared<op::Parameter>(element::f32, Shape{7});
    auto add0 = make_shared<op::Add>();
    auto abs0 = make_shared<op::Abs>();
    auto acos0 = make_shared<op::Acos>();
    auto add1 = make_shared<op::Add>();
    add0->set_argument(1, arg0);
    add0->set_argument(0, arg1);
    abs0->set_argument(0, add0);
    acos0->set_argument(0, add0);
    add1->set_argument(0, acos0);
    add1->set_argument(1, abs0);
    NodeVector ops{arg0, arg1, add0, abs0, acos0, add1};
    validate_nodes_and_infer_types(ops);
    ASSERT_EQ(add1->get_output_shape(0), Shape{7});
}

TEST(build_graph, multi_output_split)
{
    const auto data = make_shared<op::Parameter>(element::f32, Shape{64, 8, 100, 150});
    auto filters = make_shared<op::Parameter>(element::f32, Shape{128, 2, 10, 20});
    const auto split = make_shared<op::Split>(data, 1, 2);
    auto conv = make_shared<op::GroupConvolution>(split->output(1),
                                                  filters,
                                                  Strides{1, 1},
                                                  Strides{1, 1},
                                                  CoordinateDiff{0, 0},
                                                  CoordinateDiff{0, 0},
                                                  Strides{1, 1},
                                                  2);
    EXPECT_EQ(conv->get_shape(), (Shape{64, 128, 91, 131}));
}
