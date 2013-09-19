/* This code is generated by conversiongenerator.py.
 * I do not recommend editing it.
 * To update, run: python2 conversionGenerator.py > generated.h
 */
    
class PythonAstTransformer {
public:
    CodeAst* ast;
    KDevPG::MemoryPool* m_pool;
    PythonAstTransformer(int lineOffset, KDevPG::MemoryPool* pool) :  m_pool(pool), m_lineOffset(lineOffset) {};
    void run(mod_ty syntaxtree, QString moduleName) {
        ast = new (m_pool->allocate(sizeof(CodeAst))) CodeAst();
        ast->name = new (m_pool->allocate(sizeof(Identifier))) Identifier(moduleName);
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



    Ast* visitNode(_expr* node) {
        if ( ! node ) return 0;
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        Ast* result = 0;
        switch ( node->kind ) {
        case BoolOp_kind: {
                BooleanOperationAst* v = new (m_pool->allocate(sizeof(BooleanOperationAst))) BooleanOperationAst(parent());
                v->type = (ExpressionAst::BooleanOperationTypes) node->v.BoolOp.op;
                nodeStack.push(v); v->values = visitNodeList<_expr, ExpressionAst>(node->v.BoolOp.values); nodeStack.pop();
                result = v;
                break;
            }
        case BinOp_kind: {
                BinaryOperationAst* v = new (m_pool->allocate(sizeof(BinaryOperationAst))) BinaryOperationAst(parent());
                v->type = (ExpressionAst::OperatorTypes) node->v.BinOp.op;
                nodeStack.push(v); v->lhs = static_cast<ExpressionAst*>(visitNode(node->v.BinOp.left)); nodeStack.pop();
                nodeStack.push(v); v->rhs = static_cast<ExpressionAst*>(visitNode(node->v.BinOp.right)); nodeStack.pop();
                result = v;
                break;
            }
        case UnaryOp_kind: {
                UnaryOperationAst* v = new (m_pool->allocate(sizeof(UnaryOperationAst))) UnaryOperationAst(parent());
                v->type = (ExpressionAst::UnaryOperatorTypes) node->v.UnaryOp.op;
                nodeStack.push(v); v->operand = static_cast<ExpressionAst*>(visitNode(node->v.UnaryOp.operand)); nodeStack.pop();
                result = v;
                break;
            }
        case Lambda_kind: {
                LambdaAst* v = new (m_pool->allocate(sizeof(LambdaAst))) LambdaAst(parent());
                nodeStack.push(v); v->arguments = static_cast<ArgumentsAst*>(visitNode(node->v.Lambda.args)); nodeStack.pop();
                nodeStack.push(v); v->body = static_cast<ExpressionAst*>(visitNode(node->v.Lambda.body)); nodeStack.pop();
                result = v;
                break;
            }
        case IfExp_kind: {
                IfExpressionAst* v = new (m_pool->allocate(sizeof(IfExpressionAst))) IfExpressionAst(parent());
                nodeStack.push(v); v->condition = static_cast<ExpressionAst*>(visitNode(node->v.IfExp.test)); nodeStack.pop();
                nodeStack.push(v); v->body = static_cast<ExpressionAst*>(visitNode(node->v.IfExp.body)); nodeStack.pop();
                nodeStack.push(v); v->orelse = static_cast<ExpressionAst*>(visitNode(node->v.IfExp.orelse)); nodeStack.pop();
                result = v;
                break;
            }
        case Dict_kind: {
                DictAst* v = new (m_pool->allocate(sizeof(DictAst))) DictAst(parent());
                nodeStack.push(v); v->keys = visitNodeList<_expr, ExpressionAst>(node->v.Dict.keys); nodeStack.pop();
                nodeStack.push(v); v->values = visitNodeList<_expr, ExpressionAst>(node->v.Dict.values); nodeStack.pop();
                result = v;
                break;
            }
        case ListComp_kind: {
                ListComprehensionAst* v = new (m_pool->allocate(sizeof(ListComprehensionAst))) ListComprehensionAst(parent());
                nodeStack.push(v); v->element = static_cast<ExpressionAst*>(visitNode(node->v.ListComp.elt)); nodeStack.pop();
                nodeStack.push(v); v->generators = visitNodeList<_comprehension, ComprehensionAst>(node->v.ListComp.generators); nodeStack.pop();
                result = v;
                break;
            }
        case DictComp_kind: {
                DictionaryComprehensionAst* v = new (m_pool->allocate(sizeof(DictionaryComprehensionAst))) DictionaryComprehensionAst(parent());
                nodeStack.push(v); v->key = static_cast<ExpressionAst*>(visitNode(node->v.DictComp.key)); nodeStack.pop();
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.DictComp.value)); nodeStack.pop();
                nodeStack.push(v); v->generators = visitNodeList<_comprehension, ComprehensionAst>(node->v.DictComp.generators); nodeStack.pop();
                result = v;
                break;
            }
        case GeneratorExp_kind: {
                GeneratorExpressionAst* v = new (m_pool->allocate(sizeof(GeneratorExpressionAst))) GeneratorExpressionAst(parent());
                nodeStack.push(v); v->element = static_cast<ExpressionAst*>(visitNode(node->v.GeneratorExp.elt)); nodeStack.pop();
                nodeStack.push(v); v->generators = visitNodeList<_comprehension, ComprehensionAst>(node->v.GeneratorExp.generators); nodeStack.pop();
                result = v;
                break;
            }
        case Yield_kind: {
                YieldAst* v = new (m_pool->allocate(sizeof(YieldAst))) YieldAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Yield.value)); nodeStack.pop();
                result = v;
                break;
            }
        case Compare_kind: {
                CompareAst* v = new (m_pool->allocate(sizeof(CompareAst))) CompareAst(parent());
                nodeStack.push(v); v->leftmostElement = static_cast<ExpressionAst*>(visitNode(node->v.Compare.left)); nodeStack.pop();

                for ( int _i = 0; _i < node->v.Compare.ops->size; _i++ ) {
                    v->operators.append((ExpressionAst::ComparisonOperatorTypes) node->v.Compare.ops->elements[_i]);
                }

                nodeStack.push(v); v->comparands = visitNodeList<_expr, ExpressionAst>(node->v.Compare.comparators); nodeStack.pop();
                result = v;
                break;
            }
        case Call_kind: {
                CallAst* v = new (m_pool->allocate(sizeof(CallAst))) CallAst(parent());
                nodeStack.push(v); v->function = static_cast<ExpressionAst*>(visitNode(node->v.Call.func)); nodeStack.pop();
                nodeStack.push(v); v->arguments = visitNodeList<_expr, ExpressionAst>(node->v.Call.args); nodeStack.pop();
                nodeStack.push(v); v->keywords = visitNodeList<_keyword, KeywordAst>(node->v.Call.keywords); nodeStack.pop();
                nodeStack.push(v); v->keywordArguments = static_cast<ExpressionAst*>(visitNode(node->v.Call.kwargs)); nodeStack.pop();
                nodeStack.push(v); v->starArguments = static_cast<ExpressionAst*>(visitNode(node->v.Call.starargs)); nodeStack.pop();
v->function->belongsToCall = v;
                result = v;
                break;
            }
        case Repr_kind: {
                ReprAst* v = new (m_pool->allocate(sizeof(ReprAst))) ReprAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Repr.value)); nodeStack.pop();
                result = v;
                break;
            }
        case Num_kind: {
                NumberAst* v = new (m_pool->allocate(sizeof(NumberAst))) NumberAst(parent());
v->isInt = PyInt_Check(node->v.Num.n); v->value = PyInt_AsLong(node->v.Num.n);
                result = v;
                break;
            }
        case Str_kind: {
                StringAst* v = new (m_pool->allocate(sizeof(StringAst))) StringAst(parent());
                v->value = QString::fromUtf8(PyString_AsString(PyObject_Str(node->v.Str.s)));
                result = v;
                break;
            }
        case Attribute_kind: {
                AttributeAst* v = new (m_pool->allocate(sizeof(AttributeAst))) AttributeAst(parent());
                v->attribute = node->v.Attribute.attr ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->v.Attribute.attr)))) : 0;
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
                SubscriptAst* v = new (m_pool->allocate(sizeof(SubscriptAst))) SubscriptAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Subscript.value)); nodeStack.pop();
                nodeStack.push(v); v->slice = static_cast<SliceAst*>(visitNode(node->v.Subscript.slice)); nodeStack.pop();
                v->context = (ExpressionAst::Context) node->v.Subscript.ctx;
                result = v;
                break;
            }
        case Name_kind: {
                NameAst* v = new (m_pool->allocate(sizeof(NameAst))) NameAst(parent());
                v->identifier = node->v.Name.id ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->v.Name.id)))) : 0;
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
                ListAst* v = new (m_pool->allocate(sizeof(ListAst))) ListAst(parent());
                nodeStack.push(v); v->elements = visitNodeList<_expr, ExpressionAst>(node->v.List.elts); nodeStack.pop();
                v->context = (ExpressionAst::Context) node->v.List.ctx;
                result = v;
                break;
            }
        case Tuple_kind: {
                TupleAst* v = new (m_pool->allocate(sizeof(TupleAst))) TupleAst(parent());
                nodeStack.push(v); v->elements = visitNodeList<_expr, ExpressionAst>(node->v.Tuple.elts); nodeStack.pop();
                v->context = (ExpressionAst::Context) node->v.Tuple.ctx;
                result = v;
                break;
            }
        case Set_kind: {
                SetAst* v = new (m_pool->allocate(sizeof(SetAst))) SetAst(parent());
                nodeStack.push(v); v->elements = visitNodeList<_expr, ExpressionAst>(node->v.Set.elts); nodeStack.pop();
                result = v;
                break;
            }
        case SetComp_kind: {
                SetComprehensionAst* v = new (m_pool->allocate(sizeof(SetComprehensionAst))) SetComprehensionAst(parent());
                nodeStack.push(v); v->element = static_cast<ExpressionAst*>(visitNode(node->v.SetComp.elt)); nodeStack.pop();
                nodeStack.push(v); v->generators = visitNodeList<_comprehension, ComprehensionAst>(node->v.SetComp.generators); nodeStack.pop();
                result = v;
                break;
            }
        default:
            kWarning() << "Unsupported statement AST type: " << node->kind;
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
    
        NameAst* r = dynamic_cast<NameAst*>(result);
        if ( r ) {
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
                ExceptionHandlerAst* v = new (m_pool->allocate(sizeof(ExceptionHandlerAst))) ExceptionHandlerAst(parent());
                nodeStack.push(v); v->type = static_cast<ExpressionAst*>(visitNode(node->v.ExceptHandler.type)); nodeStack.pop();
                nodeStack.push(v); v->name = static_cast<ExpressionAst*>(visitNode(node->v.ExceptHandler.name)); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.ExceptHandler.body); nodeStack.pop();
                result = v;
                break;
            }
        default:
            kWarning() << "Unsupported statement AST type: " << node->kind;
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
    
        NameAst* r = dynamic_cast<NameAst*>(result);
        if ( r ) {
            r->startCol = r->identifier->startCol;
            r->endCol = r->identifier->endCol;
            r->startLine = r->identifier->startLine;
            r->endLine = r->identifier->endLine;
        }
        return result;
    }


    Ast* visitNode(_comprehension* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0; // return a nullpointer if no node is set, that's fine, everyone else will check for that.
                ComprehensionAst* v = new (m_pool->allocate(sizeof(ComprehensionAst))) ComprehensionAst(parent());
            nodeStack.push(v); v->target = static_cast<ExpressionAst*>(visitNode(node->target)); nodeStack.pop();
            nodeStack.push(v); v->iterator = static_cast<ExpressionAst*>(visitNode(node->iter)); nodeStack.pop();
            nodeStack.push(v); v->conditions = visitNodeList<_expr, ExpressionAst>(node->ifs); nodeStack.pop();
        return v;
    }


    Ast* visitNode(_alias* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0; // return a nullpointer if no node is set, that's fine, everyone else will check for that.
                AliasAst* v = new (m_pool->allocate(sizeof(AliasAst))) AliasAst(parent());
            v->name = node->name ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->name)))) : 0;
                if ( v->name ) {
                    v->name->startCol = node->col_offset; v->startCol = v->name->startCol;
                    v->name->startLine = tline(node->lineno - 1);  v->startLine = v->name->startLine;
                    v->name->endCol = node->col_offset + v->name->value.length() - 1;  v->endCol = v->name->endCol;
                    v->name->endLine = tline(node->lineno - 1);  v->endLine = v->name->endLine;
                    ranges_copied = true;
                }
            v->asName = node->asname ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->asname)))) : 0;
                if ( v->asName ) {
                    v->asName->startCol = node->col_offset; v->startCol = v->asName->startCol;
                    v->asName->startLine = tline(node->lineno - 1);  v->startLine = v->asName->startLine;
                    v->asName->endCol = node->col_offset + v->asName->value.length() - 1;  v->endCol = v->asName->endCol;
                    v->asName->endLine = tline(node->lineno - 1);  v->endLine = v->asName->endLine;
                    ranges_copied = true;
                }
        return v;
    }


    Ast* visitNode(_stmt* node) {
        if ( ! node ) return 0;
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        Ast* result = 0;
        switch ( node->kind ) {
        case Expr_kind: {
                ExpressionAst* v = new (m_pool->allocate(sizeof(ExpressionAst))) ExpressionAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Expr.value)); nodeStack.pop();
                result = v;
                break;
            }
        case FunctionDef_kind: {
                FunctionDefinitionAst* v = new (m_pool->allocate(sizeof(FunctionDefinitionAst))) FunctionDefinitionAst(parent());
                v->name = node->v.FunctionDef.name ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->v.FunctionDef.name)))) : 0;
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
                result = v;
                break;
            }
        case ClassDef_kind: {
                ClassDefinitionAst* v = new (m_pool->allocate(sizeof(ClassDefinitionAst))) ClassDefinitionAst(parent());
                v->name = node->v.ClassDef.name ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->v.ClassDef.name)))) : 0;
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
                ReturnAst* v = new (m_pool->allocate(sizeof(ReturnAst))) ReturnAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Return.value)); nodeStack.pop();
                result = v;
                break;
            }
        case Delete_kind: {
                DeleteAst* v = new (m_pool->allocate(sizeof(DeleteAst))) DeleteAst(parent());
                nodeStack.push(v); v->targets = visitNodeList<_expr, ExpressionAst>(node->v.Delete.targets); nodeStack.pop();
                result = v;
                break;
            }
        case Assign_kind: {
                AssignmentAst* v = new (m_pool->allocate(sizeof(AssignmentAst))) AssignmentAst(parent());
                nodeStack.push(v); v->targets = visitNodeList<_expr, ExpressionAst>(node->v.Assign.targets); nodeStack.pop();
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Assign.value)); nodeStack.pop();
                result = v;
                break;
            }
        case AugAssign_kind: {
                AugmentedAssignmentAst* v = new (m_pool->allocate(sizeof(AugmentedAssignmentAst))) AugmentedAssignmentAst(parent());
                nodeStack.push(v); v->target = static_cast<ExpressionAst*>(visitNode(node->v.AugAssign.target)); nodeStack.pop();
                v->op = (ExpressionAst::OperatorTypes) node->v.AugAssign.op;
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.AugAssign.value)); nodeStack.pop();
                result = v;
                break;
            }
        case Print_kind: {
                PrintAst* v = new (m_pool->allocate(sizeof(PrintAst))) PrintAst(parent());
                nodeStack.push(v); v->destination = static_cast<ExpressionAst*>(visitNode(node->v.Print.dest)); nodeStack.pop();
                nodeStack.push(v); v->values = visitNodeList<_expr, ExpressionAst>(node->v.Print.values); nodeStack.pop();
                v->newline = node->v.Print.nl;
                result = v;
                break;
            }
        case For_kind: {
                ForAst* v = new (m_pool->allocate(sizeof(ForAst))) ForAst(parent());
                nodeStack.push(v); v->target = static_cast<ExpressionAst*>(visitNode(node->v.For.target)); nodeStack.pop();
                nodeStack.push(v); v->iterator = static_cast<ExpressionAst*>(visitNode(node->v.For.iter)); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.For.body); nodeStack.pop();
                nodeStack.push(v); v->orelse = visitNodeList<_stmt, Ast>(node->v.For.orelse); nodeStack.pop();
                result = v;
                break;
            }
        case While_kind: {
                WhileAst* v = new (m_pool->allocate(sizeof(WhileAst))) WhileAst(parent());
                nodeStack.push(v); v->condition = static_cast<ExpressionAst*>(visitNode(node->v.While.test)); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.While.body); nodeStack.pop();
                nodeStack.push(v); v->orelse = visitNodeList<_stmt, Ast>(node->v.While.orelse); nodeStack.pop();
                result = v;
                break;
            }
        case If_kind: {
                IfAst* v = new (m_pool->allocate(sizeof(IfAst))) IfAst(parent());
                nodeStack.push(v); v->condition = static_cast<ExpressionAst*>(visitNode(node->v.If.test)); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.If.body); nodeStack.pop();
                nodeStack.push(v); v->orelse = visitNodeList<_stmt, Ast>(node->v.If.orelse); nodeStack.pop();
                result = v;
                break;
            }
        case With_kind: {
                WithAst* v = new (m_pool->allocate(sizeof(WithAst))) WithAst(parent());
                nodeStack.push(v); v->contextExpression = static_cast<ExpressionAst*>(visitNode(node->v.With.context_expr)); nodeStack.pop();
                nodeStack.push(v); v->optionalVars = static_cast<ExpressionAst*>(visitNode(node->v.With.optional_vars)); nodeStack.pop();
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.With.body); nodeStack.pop();
                result = v;
                break;
            }
        case Raise_kind: {
                RaiseAst* v = new (m_pool->allocate(sizeof(RaiseAst))) RaiseAst(parent());
                nodeStack.push(v); v->type = static_cast<ExpressionAst*>(visitNode(node->v.Raise.type)); nodeStack.pop();
                result = v;
                break;
            }
        case TryExcept_kind: {
                TryExceptAst* v = new (m_pool->allocate(sizeof(TryExceptAst))) TryExceptAst(parent());
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.TryExcept.body); nodeStack.pop();
                nodeStack.push(v); v->orelse = visitNodeList<_stmt, Ast>(node->v.TryExcept.orelse); nodeStack.pop();
                nodeStack.push(v); v->handlers = visitNodeList<_excepthandler, ExceptionHandlerAst>(node->v.TryExcept.handlers); nodeStack.pop();
                result = v;
                break;
            }
        case TryFinally_kind: {
                TryFinallyAst* v = new (m_pool->allocate(sizeof(TryFinallyAst))) TryFinallyAst(parent());
                nodeStack.push(v); v->body = visitNodeList<_stmt, Ast>(node->v.TryFinally.body); nodeStack.pop();
                nodeStack.push(v); v->finalbody = visitNodeList<_stmt, Ast>(node->v.TryFinally.finalbody); nodeStack.pop();
                result = v;
                break;
            }
        case Assert_kind: {
                AssertionAst* v = new (m_pool->allocate(sizeof(AssertionAst))) AssertionAst(parent());
                nodeStack.push(v); v->condition = static_cast<ExpressionAst*>(visitNode(node->v.Assert.test)); nodeStack.pop();
                nodeStack.push(v); v->message = static_cast<ExpressionAst*>(visitNode(node->v.Assert.msg)); nodeStack.pop();
                result = v;
                break;
            }
        case Import_kind: {
                ImportAst* v = new (m_pool->allocate(sizeof(ImportAst))) ImportAst(parent());
                nodeStack.push(v); v->names = visitNodeList<_alias, AliasAst>(node->v.Import.names); nodeStack.pop();
                result = v;
                break;
            }
        case ImportFrom_kind: {
                ImportFromAst* v = new (m_pool->allocate(sizeof(ImportFromAst))) ImportFromAst(parent());
                v->module = node->v.ImportFrom.module ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->v.ImportFrom.module)))) : 0;
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
        case Exec_kind: {
                ExecAst* v = new (m_pool->allocate(sizeof(ExecAst))) ExecAst(parent());
                nodeStack.push(v); v->body = static_cast<ExpressionAst*>(visitNode(node->v.Exec.body)); nodeStack.pop();
                nodeStack.push(v); v->globals = static_cast<ExpressionAst*>(visitNode(node->v.Exec.globals)); nodeStack.pop();
                nodeStack.push(v); v->locals = static_cast<ExpressionAst*>(visitNode(node->v.Exec.locals)); nodeStack.pop();
                result = v;
                break;
            }
        case Global_kind: {
                GlobalAst* v = new (m_pool->allocate(sizeof(GlobalAst))) GlobalAst(parent());

                for ( int _i = 0; _i < node->v.Global.names->size; _i++ ) {
                    Python::Identifier* id = new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(
                                    static_cast<PyObject*>(node->v.Global.names->elements[_i])
                            ))));
                    v->names.append(id);
                }

                result = v;
                break;
            }
        case Break_kind: {
                BreakAst* v = new (m_pool->allocate(sizeof(BreakAst))) BreakAst(parent());
                result = v;
                break;
            }
        case Continue_kind: {
                ContinueAst* v = new (m_pool->allocate(sizeof(ContinueAst))) ContinueAst(parent());
                result = v;
                break;
            }
        case Pass_kind: {
                PassAst* v = new (m_pool->allocate(sizeof(PassAst))) PassAst(parent());
                result = v;
                break;
            }
        default:
            kWarning() << "Unsupported statement AST type: " << node->kind;
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
    
        NameAst* r = dynamic_cast<NameAst*>(result);
        if ( r ) {
            r->startCol = r->identifier->startCol;
            r->endCol = r->identifier->endCol;
            r->startLine = r->identifier->startLine;
            r->endLine = r->identifier->endLine;
        }
        return result;
    }


    Ast* visitNode(_slice* node) {
        if ( ! node ) return 0;
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        Ast* result = 0;
        switch ( node->kind ) {
        case Slice_kind: {
                SliceAst* v = new (m_pool->allocate(sizeof(SliceAst))) SliceAst(parent());
                nodeStack.push(v); v->lower = static_cast<ExpressionAst*>(visitNode(node->v.Slice.lower)); nodeStack.pop();
                nodeStack.push(v); v->upper = static_cast<ExpressionAst*>(visitNode(node->v.Slice.upper)); nodeStack.pop();
                nodeStack.push(v); v->step = static_cast<ExpressionAst*>(visitNode(node->v.Slice.step)); nodeStack.pop();
                result = v;
                break;
            }
        case ExtSlice_kind: {
                ExtendedSliceAst* v = new (m_pool->allocate(sizeof(ExtendedSliceAst))) ExtendedSliceAst(parent());
                nodeStack.push(v); v->dims = visitNodeList<_slice, SliceAst>(node->v.ExtSlice.dims); nodeStack.pop();
                result = v;
                break;
            }
        case Index_kind: {
                IndexAst* v = new (m_pool->allocate(sizeof(IndexAst))) IndexAst(parent());
                nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->v.Index.value)); nodeStack.pop();
                result = v;
                break;
            }
        case Ellipsis_kind: {
                EllipsisAst* v = new (m_pool->allocate(sizeof(EllipsisAst))) EllipsisAst(parent());
                result = v;
                break;
            }
        default:
            kWarning() << "Unsupported statement AST type: " << node->kind;
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
    
        NameAst* r = dynamic_cast<NameAst*>(result);
        if ( r ) {
            r->startCol = r->identifier->startCol;
            r->endCol = r->identifier->endCol;
            r->startLine = r->identifier->startLine;
            r->endLine = r->identifier->endLine;
        }
        return result;
    }


    Ast* visitNode(_arguments* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0; // return a nullpointer if no node is set, that's fine, everyone else will check for that.
                ArgumentsAst* v = new (m_pool->allocate(sizeof(ArgumentsAst))) ArgumentsAst(parent());
            v->vararg = node->vararg ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->vararg)))) : 0;
            v->kwarg = node->kwarg ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->kwarg)))) : 0;
            nodeStack.push(v); v->arguments = visitNodeList<_expr, ExpressionAst>(node->args); nodeStack.pop();
            nodeStack.push(v); v->defaultValues = visitNodeList<_expr, ExpressionAst>(node->defaults); nodeStack.pop();
              v->arg_lineno = tline(node->arg_lineno - 1);
              v->arg_col_offset = node->arg_col_offset;
              v->vararg_lineno = tline(node->vararg_lineno - 1);
              v->vararg_col_offset = node->vararg_col_offset;
        return v;
    }


    Ast* visitNode(_keyword* node) {
        bool ranges_copied = false; Q_UNUSED(ranges_copied);
        if ( ! node ) return 0; // return a nullpointer if no node is set, that's fine, everyone else will check for that.
                KeywordAst* v = new (m_pool->allocate(sizeof(KeywordAst))) KeywordAst(parent());
            v->argumentName = node->arg ? new (m_pool->allocate(sizeof(Python::Identifier))) Python::Identifier(QString::fromUtf8(PyString_AsString(PyObject_Str(node->arg)))) : 0;
            nodeStack.push(v); v->value = static_cast<ExpressionAst*>(visitNode(node->value)); nodeStack.pop();
        return v;
    }

};

/*
 * End generated code
 */

