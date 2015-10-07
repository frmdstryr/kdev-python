/* This code is generated by conversiongenerator.py.
 * I do not recommend editing it.
 * To update, run: python2 conversionGenerator.py > generated.h
 */

class PythonAstTransformer {
public:
    CodeAst* ast;
    PythonAstTransformer(int lineOffset) : m_lineOffset(lineOffset) {};
    void run(mod_ty syntaxtree, QString moduleName) {
        ast = new CodeAst();
        ast->name = new Identifier(moduleName);
        nodeStack.push(ast);
        ast->body = visitNodeList<_stmt, Ast>(syntaxtree->v.Module.body);
        nodeStack.pop();
        Q_ASSERT(nodeStack.isEmpty());
    }
    // Shift lines by some fixed amount
    inline int tline(int line) {
        if ( line == -99999 ) {
            // don't touch the marker
            return -99999;
        }
        return line + m_lineOffset;
    };
private:
    QStack<Ast*> nodeStack;
    int m_lineOffset;
    
    Ast* parent() {
        return nodeStack.top();
    }
    
    template<typename T, typename K> QList<K*> visitNodeList(asdl_seq* node) {
        QList<K*> nodelist;
        if ( ! node ) return nodelist;
        for ( int i=0; i < node->size; i++ ) {
            T* currentNode = static_cast<T*>(node->elements[i]);
            Q_ASSERT(currentNode);
            Ast* result = visitNode(currentNode);
            K* transformedNode = static_cast<K*>(result);
            nodelist.append(transformedNode);
        }
        return nodelist;
    }



    Ast* visitNode(_slice* node) {
        if ( ! node ) return 0;
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        Ast* result = 0;
        switch ( node->kind ) {
        case Slice_kind: {
                SliceAst* v = new  SliceAst(parent());
                nodeStack.push(v); v->lower = static_cast<ExpressionAst*>(visitNode(node->v.Slice.lower)); nodeStack.pop();
                nodeStack.push(v); v->upper = static_cast<ExpressionAst*>(visitNode(node->v.Slice.upper)); nodeStack.pop();
                nodeStack.push(v); v->step = static_cast<ExpressionAst*>(visitNode(node->v.Slice.step)); nodeStack.pop();
                result = v;
                break;
            }
        case ExtSlice_kind: {
                ExtendedSliceAst* v = new  ExtendedSliceAst(parent());
                nodeStack.push(v); v->dims = visitNodeList<_slice, SliceAst>(node->v.ExtSlice.dims); nodeStack.pop();
                result = v;
                break;
            }
        case Index_kind: {
                IndexAst* v = new  IndexAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Index.value)); nodeStack.pop();
                result = v;
                break;
            }
        default:
            qWarning() << "Unsupported statement AST type: " << node->kind;
            Q_ASSERT(false);
        }

        // Walk through the tree and set proper end columns and lines, as the python parser sadly does not do this for us
        if ( result->hasUsefulRangeInformation ) {
            Ast* parent = result->parent;
            while ( parent ) {
                if ( parent->endLine < result->endLine ) {
                    parent->endLine = result->endLine;
                    parent->endCol = result->endCol;
                }
                if ( ! parent->hasUsefulRangeInformation && parent->startLine == -99999 ) {
                    parent->startLine = result->startLine;
                    parent->startCol = result->startCol;
                }
                parent = parent->parent;
            }
        }
    
        if ( result && result->astType == Ast::NameAstType ) {
            NameAst* r = static_cast<NameAst*>(result);
            r->startCol = r->identifier->startCol;
            r->endCol = r->identifier->endCol;
            r->startLine = r->identifier->startLine;
            r->endLine = r->identifier->endLine;
        }
        return result;
    }


    Ast* visitNode(_arguments* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0;
                ArgumentsAst* v = new  ArgumentsAst(parent());
            nodeStack.push(v); v->vararg = static_cast<ArgAst*>(visitNode(node->vararg)); nodeStack.pop();
            nodeStack.push(v); v->kwarg = static_cast<ArgAst*>(visitNode(node->kwarg)); nodeStack.pop();
            nodeStack.push(v); v->arguments = visitNodeList<_arg, ArgAst>(node->args); nodeStack.pop();
            nodeStack.push(v); v->defaultValues = visitNodeList<_expr, ExpressionAst>(node->defaults); nodeStack.pop();
        return v;
    }


    Ast* visitNode(_keyword* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0;
                KeywordAst* v = new  KeywordAst(parent());
            v->argumentName = node->arg ? new Python::Identifier(PyUnicodeObjectToQString(node->arg)) : 0;
            nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->value)); nodeStack.pop();
        return v;
    }


    Ast* visitNode(_withitem* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0;
                WithItemAst* v = new  WithItemAst(parent());
            nodeStack.push(v); v->contextExpression = static_cast<ExpressionAst*>(visitNode(node->context_expr)); nodeStack.pop();
            nodeStack.push(v); v->optionalVars = static_cast<NameAst*>(visitNode(node->optional_vars)); nodeStack.pop();
        return v;
    }


    Ast* visitNode(_expr* node) {
        if ( ! node ) return 0;
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        Ast* result = 0;
        switch ( node->kind ) {
        case BoolOp_kind: {
                BooleanOperationAst* v = new  BooleanOperationAst(parent());
                v->type = (ExpressionAst::BooleanOperationTypes) node->v.BoolOp.op;
                nodeStack.push(v); v->values = visitNodeList<_expr, ExpressionAst>(node->v.BoolOp.values); nodeStack.pop();
                result = v;
                break;
            }
        case BinOp_kind: {
                BinaryOperationAst* v = new  BinaryOperationAst(parent());
                v->type = (ExpressionAst::OperatorTypes) node->v.BinOp.op;
                nodeStack.push(v); v->lhs = static_cast<ExpressionAst*>(visitNode(node->v.BinOp.left)); nodeStack.pop();
                nodeStack.push(v); v->rhs = static_cast<ExpressionAst*>(visitNode(node->v.BinOp.right)); nodeStack.pop();
                result = v;
                break;
            }
        case UnaryOp_kind: {
                UnaryOperationAst* v = new  UnaryOperationAst(parent());
                v->type = (ExpressionAst::UnaryOperatorTypes) node->v.UnaryOp.op;
                nodeStack.push(v); v->operand = static_cast<ExpressionAst*>(visitNode(node->v.UnaryOp.operand)); nodeStack.pop();
                result = v;
                break;
            }
        case Lambda_kind: {
                LambdaAst* v = new  LambdaAst(parent());
                nodeStack.push(v); v->arguments = static_cast<ArgumentsAst*>(visitNode(node->v.Lambda.args)); nodeStack.pop();
                nodeStack.push(v); v->body = static_cast<ExpressionAst*>(visitNode(node->v.Lambda.body)); nodeStack.pop();
                result = v;
                break;
            }
        case IfExp_kind: {
                IfExpressionAst* v = new  IfExpressionAst(parent());
                nodeStack.push(v); v->condition = static_cast<ExpressionAst*>(visitNode(node->v.IfExp.test)); nodeStack.pop();
                nodeStack.push(v); v->body = static_cast<ExpressionAst*>(visitNode(node->v.IfExp.body)); nodeStack.pop();
                nodeStack.push(v); v->orelse = static_cast<ExpressionAst*>(visitNode(node->v.IfExp.orelse)); nodeStack.pop();
                result = v;
                break;
            }
        case Dict_kind: {
                DictAst* v = new  DictAst(parent());
                nodeStack.push(v); v->keys = visitNodeList<_expr, ExpressionAst>(node->v.Dict.keys); nodeStack.pop();
                nodeStack.push(v); v->values = visitNodeList<_expr, ExpressionAst>(node->v.Dict.values); nodeStack.pop();
                result = v;
                break;
            }
        case Set_kind: {
                SetAst* v = new  SetAst(parent());
                nodeStack.push(v); v->elements = visitNodeList<_expr, ExpressionAst>(node->v.Set.elts); nodeStack.pop();
                result = v;
                break;
            }
        case ListComp_kind: {
                ListComprehensionAst* v = new  ListComprehensionAst(parent());
                nodeStack.push(v); v->element = static_cast<ExpressionAst*>(visitNode(node->v.ListComp.elt)); nodeStack.pop();
                nodeStack.push(v); v->generators = visitNodeList<_comprehension, ComprehensionAst>(node->v.ListComp.generators); nodeStack.pop();
                result = v;
                break;
            }
        case SetComp_kind: {
                SetComprehensionAst* v = new  SetComprehensionAst(parent());
                nodeStack.push(v); v->element = static_cast<ExpressionAst*>(visitNode(node->v.SetComp.elt)); nodeStack.pop();
                nodeStack.push(v); v->generators = visitNodeList<_comprehension, ComprehensionAst>(node->v.SetComp.generators); nodeStack.pop();
                result = v;
                break;
            }
        case DictComp_kind: {
                DictionaryComprehensionAst* v = new  DictionaryComprehensionAst(parent());
                nodeStack.push(v); v->key = static_cast<ExpressionAst*>(visitNode(node->v.DictComp.key)); nodeStack.pop();
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.DictComp.value)); nodeStack.pop();
                nodeStack.push(v); v->generators = visitNodeList<_comprehension, ComprehensionAst>(node->v.DictComp.generators); nodeStack.pop();
                result = v;
                break;
            }
        case GeneratorExp_kind: {
                GeneratorExpressionAst* v = new  GeneratorExpressionAst(parent());
                nodeStack.push(v); v->element = static_cast<ExpressionAst*>(visitNode(node->v.GeneratorExp.elt)); nodeStack.pop();
                nodeStack.push(v); v->generators = visitNodeList<_comprehension, ComprehensionAst>(node->v.GeneratorExp.generators); nodeStack.pop();
                result = v;
                break;
            }
        case Yield_kind: {
                YieldAst* v = new  YieldAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Yield.value)); nodeStack.pop();
                result = v;
                break;
            }
        case Compare_kind: {
                CompareAst* v = new  CompareAst(parent());
                nodeStack.push(v); v->leftmostElement = static_cast<ExpressionAst*>(visitNode(node->v.Compare.left)); nodeStack.pop();

                for ( int _i = 0; _i < node->v.Compare.ops->size; _i++ ) {
                    v->operators.append((ExpressionAst::ComparisonOperatorTypes) node->v.Compare.ops->elements[_i]);
                }

                nodeStack.push(v); v->comparands = visitNodeList<_expr, ExpressionAst>(node->v.Compare.comparators); nodeStack.pop();
                result = v;
                break;
            }
        case Call_kind: {
                CallAst* v = new  CallAst(parent());
                nodeStack.push(v); v->function = static_cast<ExpressionAst*>(visitNode(node->v.Call.func)); nodeStack.pop();
                nodeStack.push(v); v->arguments = visitNodeList<_expr, ExpressionAst>(node->v.Call.args); nodeStack.pop();
                nodeStack.push(v); v->keywords = visitNodeList<_keyword, KeywordAst>(node->v.Call.keywords); nodeStack.pop();
                result = v;
                break;
            }
        case Num_kind: {
                NumberAst* v = new  NumberAst(parent());
 v->isInt = PyLong_Check(node->v.Num.n); v->value = PyLong_AsLong(node->v.Num.n);
                result = v;
                break;
            }
        case Str_kind: {
                StringAst* v = new  StringAst(parent());
                v->value = PyUnicodeObjectToQString(node->v.Str.s);
                result = v;
                break;
            }
        case Bytes_kind: {
                BytesAst* v = new  BytesAst(parent());
                v->value = PyUnicodeObjectToQString(node->v.Bytes.s);
                result = v;
                break;
            }
        case Attribute_kind: {
                AttributeAst* v = new  AttributeAst(parent());
                v->attribute = node->v.Attribute.attr ? new Python::Identifier(PyUnicodeObjectToQString(node->v.Attribute.attr)) : 0;
                if ( v->attribute ) {
                    v->attribute->startCol = node->col_offset; v->startCol = v->attribute->startCol;
                    v->attribute->startLine = tline(node->lineno - 1);  v->startLine = v->attribute->startLine;
                    v->attribute->endCol = node->col_offset + v->attribute->value.length() - 1;  v->endCol = v->attribute->endCol;
                    v->attribute->endLine = tline(node->lineno - 1);  v->endLine = v->attribute->endLine;
                    ranges_copied = true;
                }
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Attribute.value)); nodeStack.pop();
                v->context = (ExpressionAst::Context) node->v.Attribute.ctx;
                result = v;
                break;
            }
        case Subscript_kind: {
                SubscriptAst* v = new  SubscriptAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Subscript.value)); nodeStack.pop();
                nodeStack.push(v); v->slice = static_cast<SliceAst*>(visitNode(node->v.Subscript.slice)); nodeStack.pop();
                v->context = (ExpressionAst::Context) node->v.Subscript.ctx;
                result = v;
                break;
            }
        case Starred_kind: {
                StarredAst* v = new  StarredAst(parent());
                result = v;
                break;
            }
        case Name_kind: {
                NameAst* v = new  NameAst(parent());
                v->identifier = node->v.Name.id ? new Python::Identifier(PyUnicodeObjectToQString(node->v.Name.id)) : 0;
                if ( v->identifier ) {
                    v->identifier->startCol = node->col_offset; v->startCol = v->identifier->startCol;
                    v->identifier->startLine = tline(node->lineno - 1);  v->startLine = v->identifier->startLine;
                    v->identifier->endCol = node->col_offset + v->identifier->value.length() - 1;  v->endCol = v->identifier->endCol;
                    v->identifier->endLine = tline(node->lineno - 1);  v->endLine = v->identifier->endLine;
                    ranges_copied = true;
                }
                v->context = (ExpressionAst::Context) node->v.Name.ctx;
                result = v;
                break;
            }
        case List_kind: {
                ListAst* v = new  ListAst(parent());
                nodeStack.push(v); v->elements = visitNodeList<_expr, ExpressionAst>(node->v.List.elts); nodeStack.pop();
                v->context = (ExpressionAst::Context) node->v.List.ctx;
                result = v;
                break;
            }
        case Tuple_kind: {
                TupleAst* v = new  TupleAst(parent());
                nodeStack.push(v); v->elements = visitNodeList<_expr, ExpressionAst>(node->v.Tuple.elts); nodeStack.pop();
                v->context = (ExpressionAst::Context) node->v.Tuple.ctx;
                result = v;
                break;
            }
        case Ellipsis_kind: {
                EllipsisAst* v = new  EllipsisAst(parent());
                result = v;
                break;
            }
        case NameConstant_kind: {
                NameConstantAst* v = new  NameConstantAst(parent());
                v->value = node->v.NameConstant.value == Py_None ? NameConstantAst::None : node->v.NameConstant.value == Py_False ? NameConstantAst::False : NameConstantAst::True;
                result = v;
                break;
            }
        case YieldFrom_kind: {
                YieldFromAst* v = new  YieldFromAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.YieldFrom.value)); nodeStack.pop();
                result = v;
                break;
            }
        default:
            qWarning() << "Unsupported statement AST type: " << node->kind;
            Q_ASSERT(false);
        }

	if ( ! result ) return 0;
        if ( ! ranges_copied ) {
            result->startCol = node->col_offset;
            result->endCol = node->col_offset;
            result->startLine = tline(node->lineno - 1);
            result->endLine = tline(node->lineno - 1);
            result->hasUsefulRangeInformation = true;
        }
        else {
            result->hasUsefulRangeInformation = true;
        }
        
        // Walk through the tree and set proper end columns and lines, as the python parser sadly does not do this for us
        if ( result->hasUsefulRangeInformation ) {
            Ast* parent = result->parent;
            while ( parent ) {
                if ( parent->endLine < result->endLine ) {
                    parent->endLine = result->endLine;
                    parent->endCol = result->endCol;
                }
                if ( ! parent->hasUsefulRangeInformation && parent->startLine == -99999 ) {
                    parent->startLine = result->startLine;
                    parent->startCol = result->startCol;
                }
                parent = parent->parent;
            }
        }
    
        if ( result && result->astType == Ast::NameAstType ) {
            NameAst* r = static_cast<NameAst*>(result);
            r->startCol = r->identifier->startCol;
            r->endCol = r->identifier->endCol;
            r->startLine = r->identifier->startLine;
            r->endLine = r->identifier->endLine;
        }
        return result;
    }


    Ast* visitNode(_stmt* node) {
        if ( ! node ) return 0;
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        Ast* result = 0;
        switch ( node->kind ) {
        case Expr_kind: {
                ExpressionAst* v = new  ExpressionAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Expr.value)); nodeStack.pop();
                result = v;
                break;
            }
        case FunctionDef_kind: {
                FunctionDefinitionAst* v = new  FunctionDefinitionAst(parent());
                v->name = node->v.FunctionDef.name ? new Python::Identifier(PyUnicodeObjectToQString(node->v.FunctionDef.name)) : 0;
                if ( v->name ) {
                    v->name->startCol = node->col_offset; v->startCol = v->name->startCol;
                    v->name->startLine = tline(node->lineno - 1);  v->startLine = v->name->startLine;
                    v->name->endCol = node->col_offset + v->name->value.length() - 1;  v->endCol = v->name->endCol;
                    v->name->endLine = tline(node->lineno - 1);  v->endLine = v->name->endLine;
                    ranges_copied = true;
                }
                nodeStack.push(v); v->arguments = static_cast<ArgumentsAst*>(visitNode(node->v.FunctionDef.args)); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.FunctionDef.body); nodeStack.pop();
                nodeStack.push(v); v->decorators = visitNodeList<_expr, ExpressionAst>(node->v.FunctionDef.decorator_list); nodeStack.pop();
                nodeStack.push(v); v->returns = static_cast<ExpressionAst*>(visitNode(node->v.FunctionDef.returns)); nodeStack.pop();
                result = v;
                break;
            }
        case ClassDef_kind: {
                ClassDefinitionAst* v = new  ClassDefinitionAst(parent());
                v->name = node->v.ClassDef.name ? new Python::Identifier(PyUnicodeObjectToQString(node->v.ClassDef.name)) : 0;
                if ( v->name ) {
                    v->name->startCol = node->col_offset; v->startCol = v->name->startCol;
                    v->name->startLine = tline(node->lineno - 1);  v->startLine = v->name->startLine;
                    v->name->endCol = node->col_offset + v->name->value.length() - 1;  v->endCol = v->name->endCol;
                    v->name->endLine = tline(node->lineno - 1);  v->endLine = v->name->endLine;
                    ranges_copied = true;
                }
                nodeStack.push(v); v->baseClasses = visitNodeList<_expr, ExpressionAst>(node->v.ClassDef.bases); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.ClassDef.body); nodeStack.pop();
                nodeStack.push(v); v->decorators = visitNodeList<_expr, ExpressionAst>(node->v.ClassDef.decorator_list); nodeStack.pop();
                result = v;
                break;
            }
        case Return_kind: {
                ReturnAst* v = new  ReturnAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Return.value)); nodeStack.pop();
                result = v;
                break;
            }
        case Delete_kind: {
                DeleteAst* v = new  DeleteAst(parent());
                nodeStack.push(v); v->targets = visitNodeList<_expr, ExpressionAst>(node->v.Delete.targets); nodeStack.pop();
                result = v;
                break;
            }
        case Assign_kind: {
                AssignmentAst* v = new  AssignmentAst(parent());
                nodeStack.push(v); v->targets = visitNodeList<_expr, ExpressionAst>(node->v.Assign.targets); nodeStack.pop();
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Assign.value)); nodeStack.pop();
                result = v;
                break;
            }
        case AugAssign_kind: {
                AugmentedAssignmentAst* v = new  AugmentedAssignmentAst(parent());
                nodeStack.push(v); v->target = static_cast<ExpressionAst*>(visitNode(node->v.AugAssign.target)); nodeStack.pop();
                v->op = (ExpressionAst::OperatorTypes) node->v.AugAssign.op;
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.AugAssign.value)); nodeStack.pop();
                result = v;
                break;
            }
        case For_kind: {
                ForAst* v = new  ForAst(parent());
                nodeStack.push(v); v->target = static_cast<ExpressionAst*>(visitNode(node->v.For.target)); nodeStack.pop();
                nodeStack.push(v); v->iterator = static_cast<ExpressionAst*>(visitNode(node->v.For.iter)); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.For.body); nodeStack.pop();
                nodeStack.push(v); v->orelse = visitNodeList<_stmt, Ast>(node->v.For.orelse); nodeStack.pop();
                result = v;
                break;
            }
        case While_kind: {
                WhileAst* v = new  WhileAst(parent());
                nodeStack.push(v); v->condition = static_cast<ExpressionAst*>(visitNode(node->v.While.test)); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.While.body); nodeStack.pop();
                nodeStack.push(v); v->orelse = visitNodeList<_stmt, Ast>(node->v.While.orelse); nodeStack.pop();
                result = v;
                break;
            }
        case If_kind: {
                IfAst* v = new  IfAst(parent());
                nodeStack.push(v); v->condition = static_cast<ExpressionAst*>(visitNode(node->v.If.test)); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.If.body); nodeStack.pop();
                nodeStack.push(v); v->orelse = visitNodeList<_stmt, Ast>(node->v.If.orelse); nodeStack.pop();
                result = v;
                break;
            }
        case With_kind: {
                WithAst* v = new  WithAst(parent());
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.With.body); nodeStack.pop();
                nodeStack.push(v); v->items = visitNodeList<_withitem, WithItemAst>(node->v.With.items); nodeStack.pop();
                result = v;
                break;
            }
        case Raise_kind: {
                RaiseAst* v = new  RaiseAst(parent());
                nodeStack.push(v); v->type = static_cast<ExpressionAst*>(visitNode(node->v.Raise.exc)); nodeStack.pop();
                result = v;
                break;
            }
        case Try_kind: {
                TryAst* v = new  TryAst(parent());
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.Try.body); nodeStack.pop();
                nodeStack.push(v); v->handlers = visitNodeList<_excepthandler, ExceptionHandlerAst>(node->v.Try.handlers); nodeStack.pop();
                nodeStack.push(v); v->orelse = visitNodeList<_stmt, Ast>(node->v.Try.orelse); nodeStack.pop();
                nodeStack.push(v); v->finally = visitNodeList<_stmt, Ast>(node->v.Try.finalbody); nodeStack.pop();
                result = v;
                break;
            }
        case Assert_kind: {
                AssertionAst* v = new  AssertionAst(parent());
                nodeStack.push(v); v->condition = static_cast<ExpressionAst*>(visitNode(node->v.Assert.test)); nodeStack.pop();
                nodeStack.push(v); v->message = static_cast<ExpressionAst*>(visitNode(node->v.Assert.msg)); nodeStack.pop();
                result = v;
                break;
            }
        case Import_kind: {
                ImportAst* v = new  ImportAst(parent());
                nodeStack.push(v); v->names = visitNodeList<_alias, AliasAst>(node->v.Import.names); nodeStack.pop();
                result = v;
                break;
            }
        case ImportFrom_kind: {
                ImportFromAst* v = new  ImportFromAst(parent());
                v->module = node->v.ImportFrom.module ? new Python::Identifier(PyUnicodeObjectToQString(node->v.ImportFrom.module)) : 0;
                if ( v->module ) {
                    v->module->startCol = node->col_offset; v->startCol = v->module->startCol;
                    v->module->startLine = tline(node->lineno - 1);  v->startLine = v->module->startLine;
                    v->module->endCol = node->col_offset + v->module->value.length() - 1;  v->endCol = v->module->endCol;
                    v->module->endLine = tline(node->lineno - 1);  v->endLine = v->module->endLine;
                    ranges_copied = true;
                }
                nodeStack.push(v); v->names = visitNodeList<_alias, AliasAst>(node->v.ImportFrom.names); nodeStack.pop();
                v->level = node->v.ImportFrom.level;
                result = v;
                break;
            }
        case Global_kind: {
                GlobalAst* v = new  GlobalAst(parent());

                for ( int _i = 0; _i < node->v.Global.names->size; _i++ ) {
                    Python::Identifier* id = new Python::Identifier(PyUnicodeObjectToQString(
                                    static_cast<PyObject*>(node->v.Global.names->elements[_i])
                            ));
                    v->names.append(id);
                }

                result = v;
                break;
            }
        case Break_kind: {
                BreakAst* v = new  BreakAst(parent());
                result = v;
                break;
            }
        case Continue_kind: {
                ContinueAst* v = new  ContinueAst(parent());
                result = v;
                break;
            }
        case Pass_kind: {
                PassAst* v = new  PassAst(parent());
                result = v;
                break;
            }
        case Nonlocal_kind: {
                NonlocalAst* v = new  NonlocalAst(parent());
                result = v;
                break;
            }
        default:
            qWarning() << "Unsupported statement AST type: " << node->kind;
            Q_ASSERT(false);
        }

	if ( ! result ) return 0;
        if ( ! ranges_copied ) {
            result->startCol = node->col_offset;
            result->endCol = node->col_offset;
            result->startLine = tline(node->lineno - 1);
            result->endLine = tline(node->lineno - 1);
            result->hasUsefulRangeInformation = true;
        }
        else {
            result->hasUsefulRangeInformation = true;
        }
        
        // Walk through the tree and set proper end columns and lines, as the python parser sadly does not do this for us
        if ( result->hasUsefulRangeInformation ) {
            Ast* parent = result->parent;
            while ( parent ) {
                if ( parent->endLine < result->endLine ) {
                    parent->endLine = result->endLine;
                    parent->endCol = result->endCol;
                }
                if ( ! parent->hasUsefulRangeInformation && parent->startLine == -99999 ) {
                    parent->startLine = result->startLine;
                    parent->startCol = result->startCol;
                }
                parent = parent->parent;
            }
        }
    
        if ( result && result->astType == Ast::NameAstType ) {
            NameAst* r = static_cast<NameAst*>(result);
            r->startCol = r->identifier->startCol;
            r->endCol = r->identifier->endCol;
            r->startLine = r->identifier->startLine;
            r->endLine = r->identifier->endLine;
        }
        return result;
    }


    Ast* visitNode(_excepthandler* node) {
        if ( ! node ) return 0;
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        Ast* result = 0;
        switch ( node->kind ) {
        case ExceptHandler_kind: {
                ExceptionHandlerAst* v = new  ExceptionHandlerAst(parent());
                nodeStack.push(v); v->type = static_cast<ExpressionAst*>(visitNode(node->v.ExceptHandler.type)); nodeStack.pop();
                v->name = node->v.ExceptHandler.name ? new Python::Identifier(PyUnicodeObjectToQString(node->v.ExceptHandler.name)) : 0;
                if ( v->name ) {
                    v->name->startCol = node->col_offset; v->startCol = v->name->startCol;
                    v->name->startLine = tline(node->lineno - 1);  v->startLine = v->name->startLine;
                    v->name->endCol = node->col_offset + v->name->value.length() - 1;  v->endCol = v->name->endCol;
                    v->name->endLine = tline(node->lineno - 1);  v->endLine = v->name->endLine;
                    ranges_copied = true;
                }
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.ExceptHandler.body); nodeStack.pop();
                result = v;
                break;
            }
        default:
            qWarning() << "Unsupported statement AST type: " << node->kind;
            Q_ASSERT(false);
        }

        // Walk through the tree and set proper end columns and lines, as the python parser sadly does not do this for us
        if ( result->hasUsefulRangeInformation ) {
            Ast* parent = result->parent;
            while ( parent ) {
                if ( parent->endLine < result->endLine ) {
                    parent->endLine = result->endLine;
                    parent->endCol = result->endCol;
                }
                if ( ! parent->hasUsefulRangeInformation && parent->startLine == -99999 ) {
                    parent->startLine = result->startLine;
                    parent->startCol = result->startCol;
                }
                parent = parent->parent;
            }
        }
    
        if ( result && result->astType == Ast::NameAstType ) {
            NameAst* r = static_cast<NameAst*>(result);
            r->startCol = r->identifier->startCol;
            r->endCol = r->identifier->endCol;
            r->startLine = r->identifier->startLine;
            r->endLine = r->identifier->endLine;
        }
        return result;
    }


    Ast* visitNode(_alias* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0;
                AliasAst* v = new  AliasAst(parent());
            v->name = node->name ? new Python::Identifier(PyUnicodeObjectToQString(node->name)) : 0;
            v->asName = node->asname ? new Python::Identifier(PyUnicodeObjectToQString(node->asname)) : 0;
        return v;
    }


    Ast* visitNode(_comprehension* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0;
                ComprehensionAst* v = new  ComprehensionAst(parent());
            nodeStack.push(v); v->target = static_cast<ExpressionAst*>(visitNode(node->target)); nodeStack.pop();
            nodeStack.push(v); v->iterator = static_cast<ExpressionAst*>(visitNode(node->iter)); nodeStack.pop();
            nodeStack.push(v); v->conditions = visitNodeList<_expr, ExpressionAst>(node->ifs); nodeStack.pop();
        return v;
    }


    Ast* visitNode(_arg* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0;
                ArgAst* v = new  ArgAst(parent());
            v->argumentName = node->arg ? new Python::Identifier(PyUnicodeObjectToQString(node->arg)) : 0;
                if ( v->argumentName ) {
                    v->argumentName->startCol = node->col_offset; v->startCol = v->argumentName->startCol;
                    v->argumentName->startLine = tline(node->lineno - 1);  v->startLine = v->argumentName->startLine;
                    v->argumentName->endCol = node->col_offset + v->argumentName->value.length() - 1;  v->endCol = v->argumentName->endCol;
                    v->argumentName->endLine = tline(node->lineno - 1);  v->endLine = v->argumentName->endLine;
                    ranges_copied = true;
                }
            nodeStack.push(v); v->annotation = static_cast<ExpressionAst*>(visitNode(node->annotation)); nodeStack.pop();
        return v;
    }

};

/*
 * End generated code
 */

